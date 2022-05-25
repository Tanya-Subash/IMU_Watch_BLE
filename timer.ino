void initInterruptTimer() {  
  //  Set the timing parameters.
  //  512 Hz * 0.01 seconds = 5 ticks = 4 ticks off and 1 tick of high output
  //
  //  Repeated Count: Periodic 1-clock-cycle wide pulses with optional interrupts.
  //  The last parameter to am_hal_ctimer_period_set() has no effect in this mode.
  
  setupTimerA(interruptTimer, 4); // timerNum, number of ticks 
  am_hal_ctimer_start(interruptTimer, AM_HAL_CTIMER_TIMERA);
}

void setupTimerA(int timerNum, uint32_t periodTicks)
{
  am_hal_ctimer_config_t ctimer = {
        0,
        (AM_HAL_CTIMER_FN_REPEAT | AM_HAL_CTIMER_INT_ENABLE |
         AM_HAL_CTIMER_LFRC_512HZ),
        0,
    };
  am_hal_ctimer_config(timerNum, &ctimer);
  
  am_hal_ctimer_period_set(timerNum, AM_HAL_CTIMER_TIMERA, periodTicks, 1);

  am_hal_ctimer_int_register(AM_HAL_CTIMER_INT_TIMERA0, ctimer_isr);
  am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0);
  am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA0);
  
  Serial.println("Interrupt timer configured");
}

void ctimer_isr() {
  if (isbuf1) {
    fillDataBuffer(buf1);
  }
  else {
    fillDataBuffer(buf2);
  }
}
