
#include "arkanoPi.h"

int flags = 0;

TipoSistema sistema;

// Declaracion del objeto teclado
TipoTeclado teclado = {
	.columnas = {
		// A completar por el alumno...
		// ...
		GPIO_KEYBOARD_COL_1,
		GPIO_KEYBOARD_COL_2,
		GPIO_KEYBOARD_COL_3,
		GPIO_KEYBOARD_COL_4

	},
	.filas = {
		// A completar por el alumno...
		// ...
		GPIO_KEYBOARD_ROW_1,
		GPIO_KEYBOARD_ROW_2,
		GPIO_KEYBOARD_ROW_3,
		GPIO_KEYBOARD_ROW_4
	},
	.rutinas_ISR = {
		// A completar por el alumno...
		// ...
		teclado_fila_1_isr,
		teclado_fila_2_isr,
		teclado_fila_3_isr,
		teclado_fila_4_isr
	},

	// A completar por el alumno...
	// ...
};

// Declaracion del objeto display
TipoLedDisplay led_display = {
	.pines_control_columnas = {
		// A completar por el alumno...
		// ...
		GPIO_LED_DISPLAY_COL_1,
		GPIO_LED_DISPLAY_COL_2,
		GPIO_LED_DISPLAY_COL_3,
	},
	.filas = {
		// A completar por el alumno...
		// ...
		GPIO_LED_DISPLAY_ROW_1,
		GPIO_LED_DISPLAY_ROW_2,
		GPIO_LED_DISPLAY_ROW_3,
		GPIO_LED_DISPLAY_ROW_4,
		GPIO_LED_DISPLAY_ROW_5,
		GPIO_LED_DISPLAY_ROW_6,
		GPIO_LED_DISPLAY_ROW_7,
	},
	// A completar por el alumno...
	// ...
};

//------------------------------------------------------
// FUNCIONES DE CONFIGURACION/INICIALIZACION
//------------------------------------------------------

// int ConfiguracionSistema (TipoSistema *p_sistema): procedimiento de configuracion
// e inicializacion del sistema.
// Realizará, entra otras, todas las operaciones necesarias para:
// configurar el uso de posibles librerías (e.g. Wiring Pi),
// configurar las interrupciones externas asociadas a los pines GPIO,
// configurar las interrupciones periódicas y sus correspondientes temporizadores,
// la inicializacion de los diferentes elementos de los que consta nuestro sistema,
// crear, si fuese necesario, los threads adicionales que pueda requerir el sistema
// como el thread de exploración del teclado del PC
int ConfiguraInicializaSistema (TipoSistema *p_sistema) {
	/*int result = 0;
	// A completar por el alumno...
	// ...

	// Lanzamos thread para exploracion del teclado convencional del PC

	result = piThreadCreate (thread_explora_teclado_PC);

	if (result != 0) {
		printf ("Thread didn't start!!!\n");
		return -1;
	}*/
	piLock(STD_IO_BUFFER_KEY);
	wiringPiSetupGpio();
	piUnlock(STD_IO_BUFFER_KEY);

	if(wiringPiSetupGpio() < 0){
		printf("No se ha podido configurar wiringPi\n");
		return -1;
	}
	pinMode(GPIO_ARDUINO_1, OUTPUT);
	//digitalWrite(GPIO_ARDUINO_1, HIGH);

	InicializaTeclado(&teclado);
	InicializaLedDisplay(&led_display);


	digitalWrite(GPIO_ARDUINO_1, LOW);

	piLock(STD_IO_BUFFER_KEY);

	printf ("\nBIENVENIDO A ARKANOPI\n");
	printf ("\nSeleccione el nivel de dificultad\n");
	printf ("Tecla 7 para dificultad fácil.\n");
	printf ("Tecla 8 para dificultad media.\n");
	printf ("Tecla 9 para dificultad dificil.\n");
	printf ("Tecla 0 para dificultad incremental.\n");

	piUnlock(STD_IO_BUFFER_KEY);

	return 1;
}

//------------------------------------------------------
// FUNCIONES LIGADAS A THREADS ADICIONALES
//------------------------------------------------------

PI_THREAD (thread_explora_teclado_PC) {
	int teclaPulsada;

	while(1) {
		delay(10); // Wiring Pi function: pauses program execution for at least 10 ms

		piLock (STD_IO_BUFFER_KEY);

		if(kbhit()) {

			teclaPulsada = kbread();

			switch(teclaPulsada) {
				// A completar por el alumno...
				// ...
			case 's':
					//printf("Tecla S pulsada");
					piLock (SYSTEM_FLAGS_KEY);
					flags |= FLAG_BOTON;
					piUnlock (SYSTEM_FLAGS_KEY);
					break;
				case 'd':
					//printf("Tecla D pulsada");
					piLock (SYSTEM_FLAGS_KEY);
					flags |= FLAG_MOV_DERECHA;
					piUnlock (SYSTEM_FLAGS_KEY);
					break;

				case 'a':
					piLock (SYSTEM_FLAGS_KEY);
					flags |= FLAG_MOV_IZQUIERDA;
					piUnlock (SYSTEM_FLAGS_KEY);
					break;

				case 'c':
					piLock (SYSTEM_FLAGS_KEY);
					flags |= FLAG_TIMER_JUEGO;
					piUnlock (SYSTEM_FLAGS_KEY);
					break;

				case 'q':
					exit(0);
					break;

				//case 'q':
		 			//piLock (SYSTEM_FLAGS_KEY);
					//flags |= FLAG_FIN_JUEGO;
					//piUnlock (SYSTEM_FLAGS_KEY);
					//break;


				default:
					//printf("INVALID KEY!!!\n");
					break;
			}
		}

		piUnlock (STD_IO_BUFFER_KEY);
	}
}

// wait until next_activation (absolute time)
void delay_until (unsigned int next) {
	unsigned int now = millis();
	if (next > now) {
		delay (next - now);
	}
}

int main () {
	unsigned int next;

	// Maquina de estados: lista de transiciones
	// {EstadoOrigen, CondicionDeDisparo, EstadoFinal, AccionesSiTransicion }
	fsm_trans_t arkanoPi[] = {
		{ WAIT_DIFICULTAD, CompruebaDificultadFacil, WAIT_VIDAS, DificultadFacil},
		{ WAIT_DIFICULTAD, CompruebaDificultadMedia, WAIT_VIDAS, DificultadMedia},
		{ WAIT_DIFICULTAD, CompruebaDificultadDificil, WAIT_VIDAS, DificultadDificil},
		{ WAIT_DIFICULTAD, CompruebaDificultadIncremental, WAIT_VIDAS, DificultadIncremental},
		{ WAIT_VIDAS, CompruebaJuegoConVidas, WAIT_START, JuegoConVidas},
		{ WAIT_VIDAS, CompruebaJuegoSinVidas, WAIT_START, JuegoSinVidas},
		{ WAIT_START, CompruebaBotonPulsado, WAIT_PUSH, InicializaJuego },
		{ WAIT_PUSH, CompruebaTimeoutActualizacionJuego, WAIT_PUSH, ActualizarJuego },
		{ WAIT_PUSH, CompruebaPause, WAIT_PAUSE, PauseJuego},
		{ WAIT_PAUSE, CompruebaPlay, WAIT_PUSH, PlayJuego},
		{ WAIT_PUSH, CompruebaMovimientoIzquierda, WAIT_PUSH, MuevePalaIzquierda },
		{ WAIT_PUSH, CompruebaMovimientoDerecha, WAIT_PUSH, MuevePalaDerecha },
		{ WAIT_PUSH, CompruebaFinalJuego, WAIT_END, FinalJuego },
		{ WAIT_END,  CompruebaBotonPulsado, WAIT_DIFICULTAD, ReseteaJuego },
		{-1, NULL, -1, NULL },
	};

	// Configuracion e incializacion del sistema
	ConfiguraInicializaSistema (&sistema);

	fsm_t* arkanoPi_fsm = fsm_new (WAIT_DIFICULTAD, arkanoPi, &sistema);



	// A completar por el alumno...
	// ...
	fsm_t* trans_col_fsm = fsm_new (TECLADO_ESPERA_COLUMNA, fsm_trans_excitacion_columnas, &(teclado));
	fsm_t* det_tecla_fsm = fsm_new (TECLADO_ESPERA_TECLA, fsm_trans_deteccion_pulsaciones, &(teclado));
	fsm_t* led_display_fsm = fsm_new(DISPLAY_ESPERA_COLUMNA,fsm_trans_excitacion_display, &(led_display));

	sistema.arkanoPi.tmr_actualizacion_juego = tmr_new(tmr_actualizacion_juego_isr);
	sistema.arkanoPi.p_pantalla = &(led_display.pantalla);


	next = millis();
	while (1) {
		fsm_fire (arkanoPi_fsm);

		// A completar por el alumno...
		// ...

		fsm_fire(trans_col_fsm);
		fsm_fire(det_tecla_fsm);
		fsm_fire(led_display_fsm);

		next += CLK_MS;
		delay_until (next);
	}

	tmr_destroy (sistema.arkanoPi.tmr_actualizacion_juego);
	fsm_destroy (arkanoPi_fsm);
	fsm_destroy(trans_col_fsm);
	fsm_destroy(det_tecla_fsm);
	fsm_destroy(led_display_fsm);
}
