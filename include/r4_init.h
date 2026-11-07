#pragma once

#include <Arduino.h>

extern const uint32_t __StackTop;
extern const fsp_vector_t __VECTOR_TABLE[];
extern const fsp_vector_t g_vector_table[];
void startAgt(void);

// Definiciones débiles por si el linker enlaza símbolos auxiliares de main.cpp del core (ej. al usar printf o la STL).
// Al no usarse arduino_main(), el linker las descarta del binario final (--gc-sections).
__attribute__((weak)) void setup(void) {}
__attribute__((weak)) void loop(void) {}

/**
 * Inicializa el hardware base del Arduino UNO R4 (Renesas RA4M1):
 * - Desactiva el monitor de pila (SPMON)
 * - Copia la tabla de vectores de interrupción a RAM (requerido por IRQManager)
 * - Habilita el acceso a pines (R_BSP_PinAccessEnable)
 * - Inicializa la variante de placa, ADC, USB CDC y el timer AGT (millis/delay)
 *
 * Nota: En AVR (Uno R3), init() viene definido en el core. En UNO R4 (Renesas),
 * Common.h declara `void init(void);` pero el core lo dejó dentro de `arduino_main()`.
 * Esta implementación permite usar `init()` directamente desde tu propio `int main()`.
 */
inline void init(void) {
#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__)
    __set_MSPLIM(0);
#else
    R_MPU_SPMON->SP[0].CTL = 0;
#endif

    __disable_irq();
    volatile uint32_t *irq_table = (volatile uint32_t *)((uint32_t)&__StackTop);
    for (size_t i = 0; i < BSP_CORTEX_VECTOR_TABLE_ENTRIES; i++) {
        *(irq_table + i) = (uint32_t)__VECTOR_TABLE[i];
    }
    for (size_t i = 0; i < BSP_ICU_VECTOR_MAX_ENTRIES; i++) {
        *(irq_table + i + BSP_CORTEX_VECTOR_TABLE_ENTRIES) = (uint32_t)g_vector_table[i];
    }
    SCB->VTOR = (uint32_t)irq_table;
    __DSB();
    __enable_irq();

    R_BSP_PinAccessEnable();
    initVariant();
    analogReference();
#ifndef NO_USB
    __USBStart();
    Serial.begin(115200);
#endif
    startAgt();
}
