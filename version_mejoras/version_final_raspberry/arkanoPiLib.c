#include "arkanoPiLib.h"

int ladrillos_basico[NUM_FILAS_DISPLAY][NUM_COLUMNAS_DISPLAY] = {
	{1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
};

//------------------------------------------------------
// FUNCIONES DE VISUALIZACION (ACTUALIZACION DEL OBJETO PANTALLA QUE LUEGO USARA EL DISPLAY)
//------------------------------------------------------

void PintaInstrucciones (){
	piLock(STD_IO_BUFFER_KEY);

	//printf ("\nBienvenido a ArkanoPi\n");
	printf ("\nLos controles del juego son:\n");
	printf ("Tecla 5 para comenzar el juego o resetearlo.\n");
	printf ("Tecla 4 para mover a la izquierda.\n");
	printf ("Tecla 6 para mover a la derecha.\n");
	printf ("Tecla * para pausar el juego.\n");
	printf ("Tecla # para reanudar el juego.\n");
	printf ("Tecla 1 para salir del programa.\n");
	printf ("Cuando finalices el juego pulsa la tecla 5 para volver a comenzar.\n");

	piUnlock(STD_IO_BUFFER_KEY);
}

void PintaMensajeVida (){
	piLock(STD_IO_BUFFER_KEY);
	printf ("\n");
	printf ("\n¿Quieres jugar con VIDAS?\n");
	printf ("Tecla A para jugar con 3 vidas.\n");
	printf ("Tecla B para jugar sin vidas.\n");

	piUnlock(STD_IO_BUFFER_KEY);
}

void PintaCaraTriste (fsm_t* this){
	tipo_arkanoPi* p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	piLock(MATRIX_KEY);
		for(int i=0; i<NUM_FILAS_DISPLAY; i++){
			for(int j=0; j<NUM_COLUMNAS_DISPLAY; j++){
				p_arkanoPi->p_pantalla->matriz[i][j] = pantalla_final.matriz[i][j];
			}

		}
	piUnlock(MATRIX_KEY);
}

void PintaCaraContenta (fsm_t* this){
	tipo_arkanoPi* p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	piLock(MATRIX_KEY);
		for(int i=0; i<NUM_FILAS_DISPLAY; i++){
			for(int j=0; j<NUM_COLUMNAS_DISPLAY; j++){
				p_arkanoPi->p_pantalla->matriz[i][j] = pantalla_inicial.matriz[i][j];
			}

		}
	piUnlock(MATRIX_KEY);
}

void PintaMensajeInicialPantalla (tipo_pantalla *p_pantalla, tipo_pantalla *p_pantalla_inicial) {
	int i, j = 0;

	for(i=0;i<NUM_FILAS_DISPLAY;i++) {
		for(j=0;j<NUM_COLUMNAS_DISPLAY;j++) {
			p_pantalla->matriz[i][j] = p_pantalla_inicial->matriz[i][j];
		}
	}

	return;
}

void PintaPantallaPorTerminal (tipo_pantalla *p_pantalla) {
#ifdef __SIN_PSEUDOWIRINGPI__
	int i=0, j=0;

	printf("\n[PANTALLA]\n");
	fflush(stdout);
	for(i=0;i<NUM_FILAS_DISPLAY;i++) {
		for(j=0;j<NUM_COLUMNAS_DISPLAY;j++) {
			printf("%d", p_pantalla->matriz[i][j]);
			fflush(stdout);
		}
		printf("\n");
		fflush(stdout);
	}
	fflush(stdout);
#endif
}

void ReseteaPantalla (tipo_pantalla *p_pantalla) {
	int i=0, j=0;

	for(i=0;i<NUM_FILAS_DISPLAY;i++) {
		for(j=0;j<NUM_COLUMNAS_DISPLAY;j++) {
			p_pantalla->matriz[i][j] = 0;
		}
	}

	/*piLock(STD_IO_BUFFER_KEY);

	printf ("\nBienvenido a ArkanoPi\n");
	printf ("Seleccione el nivel de dificultad\n");
	printf ("Tecla 7 para dificultad fácil.\n");
	printf ("Tecla 8 para dificultad media.\n");
	printf ("Tecla 9 para dificultad dificil.\n");
	printf ("Tecla 0 para dificultad incremental.\n");

	piUnlock(STD_IO_BUFFER_KEY);*/
}


//------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET
//------------------------------------------------------

void InicializaLadrillos(tipo_pantalla *p_ladrillos) {
	int i=0, j=0;

	for(i=0;i<NUM_FILAS_DISPLAY;i++) {
		for(j=0;j<NUM_COLUMNAS_DISPLAY;j++) {
			p_ladrillos->matriz[i][j] = ladrillos_basico[i][j];
		}
	}
}

void InicializaPelota(tipo_pelota *p_pelota) {
	// Aleatorizamos la posicion inicial de la pelota
	p_pelota->x = rand() % NUM_COLUMNAS_DISPLAY;
	p_pelota->y = 2 + rand() % (NUM_FILAS_DISPLAY-2); // 2 evita que aparezca encima de ladrillos y para que no empiece demasiado pegada al suelo de la pantalla

	// Pelota inicialmente en el centro de la pantalla
	//p_pelota->x = NUM_COLUMNAS_DISPLAY/2 - 1;
	//p_pelota->y = NUM_FILAS_DISPLAY/2 -1 ;

	InicializaPosiblesTrayectorias(p_pelota);

	// Trayectoria inicial
	//p_pelota->trayectoria.xv = 0;
	//p_pelota->trayectoria.yv = 1;
	CambiarDireccionPelota(p_pelota, rand() % p_pelota->num_posibles_trayectorias);
}

void InicializaPala(tipo_pala *p_pala) {
	// Pala inicialmente en el centro de la pantalla
	p_pala->x = NUM_COLUMNAS_DISPLAY/2 - p_pala->ancho/2;
	p_pala->y = NUM_FILAS_DISPLAY - 1;
	p_pala->ancho = NUM_COLUMNAS_PALA;
	p_pala->alto = NUM_FILAS_PALA;
}

void InicializaPosiblesTrayectorias(tipo_pelota *p_pelota) {
	p_pelota->num_posibles_trayectorias = 0;
	p_pelota->posibles_trayectorias[ARRIBA_IZQUIERDA].xv = -1;
	p_pelota->posibles_trayectorias[ARRIBA_IZQUIERDA].yv = -1;
	p_pelota->num_posibles_trayectorias++;
	p_pelota->posibles_trayectorias[ARRIBA].xv = 0;
	p_pelota->posibles_trayectorias[ARRIBA].yv = -1;
	p_pelota->num_posibles_trayectorias++;
	p_pelota->posibles_trayectorias[ARRIBA_DERECHA].xv = 1;
	p_pelota->posibles_trayectorias[ARRIBA_DERECHA].yv = -1;
	p_pelota->num_posibles_trayectorias++;
	p_pelota->posibles_trayectorias[ABAJO_DERECHA].xv = 1;
	p_pelota->posibles_trayectorias[ABAJO_DERECHA].yv = 1;
	p_pelota->num_posibles_trayectorias++;
	p_pelota->posibles_trayectorias[ABAJO].xv = 0;
	p_pelota->posibles_trayectorias[ABAJO].yv = 1;
	p_pelota->num_posibles_trayectorias++;
	p_pelota->posibles_trayectorias[ABAJO_IZQUIERDA].xv = -1;
	p_pelota->posibles_trayectorias[ABAJO_IZQUIERDA].yv = 1;
	p_pelota->num_posibles_trayectorias++;
	//p_pelota->posibles_trayectorias[IZQUIERDA].xv = -1;
	//p_pelota->posibles_trayectorias[IZQUIERDA].yv = 0;
	//p_pelota->num_posibles_trayectorias++;
	//p_pelota->posibles_trayectorias[DERECHA].xv = 1;
	//p_pelota->posibles_trayectorias[DERECHA].yv = 0;
	//p_pelota->num_posibles_trayectorias++;
}

void InicializaArduino(){
	pinMode(GPIO_ARDUINO_1, OUTPUT);
	digitalWrite(GPIO_ARDUINO_1,LOW);
}

void PintaLadrillos(tipo_pantalla *p_ladrillos, tipo_pantalla *p_pantalla) {
	int i=0, j=0;

	for(i=0;i<NUM_FILAS_DISPLAY;i++) {
		for(j=0;j<NUM_COLUMNAS_DISPLAY;j++) {
			p_pantalla->matriz[i][j] = p_ladrillos->matriz[i][j];
		}
    }
}

void PintaPala(tipo_pala *p_pala, tipo_pantalla *p_pantalla) {
	int i=0, j=0;

	for(i=0;i<NUM_FILAS_PALA;i++) {
		for(j=0;j<NUM_COLUMNAS_PALA;j++) {
			if (( (p_pala->y+i >= 0) && (p_pala->y+i < NUM_FILAS_DISPLAY) ) &&
				( (p_pala->x+j >= 0) && (p_pala->x+j < NUM_COLUMNAS_DISPLAY) ))
				p_pantalla->matriz[p_pala->y+i][p_pala->x+j] = 1;
		}
	}
}

void PintaPelota(tipo_pelota *p_pelota, tipo_pantalla *p_pantalla) {
	if( (p_pelota->x >= 0) && (p_pelota->x < NUM_COLUMNAS_DISPLAY) ) {
		if( (p_pelota->y >= 0) && (p_pelota->y < NUM_FILAS_DISPLAY) ) {
			p_pantalla->matriz[p_pelota->y][p_pelota->x] = 8;
		}
		else {
			printf("\n\nPROBLEMAS!!!! posicion y=%d de la pelota INVALIDA!!!\n\n", p_pelota->y);
			fflush(stdout);
		}
	}
	else {
		printf("\n\nPROBLEMAS!!!! posicion x=%d de la pelota INVALIDA!!!\n\n", p_pelota->x);
		fflush(stdout);
	}
}

void ActualizaPantalla(tipo_arkanoPi* p_arkanoPi) {

    // Borro toda la pantalla
	ReseteaPantalla((tipo_pantalla*)(p_arkanoPi->p_pantalla));

    // Pinta los ladrillos
	PintaLadrillos(
		(tipo_pantalla*)(&(p_arkanoPi->ladrillos)),
		(tipo_pantalla*)(p_arkanoPi->p_pantalla));

    // Pinta la pala
	PintaPala(
		(tipo_pala*)(&(p_arkanoPi->pala)),
		(tipo_pantalla*)(p_arkanoPi->p_pantalla));

	// Pinta la pelota
	PintaPelota(
		(tipo_pelota*)(&(p_arkanoPi->pelota)),
		(tipo_pantalla*)(p_arkanoPi->p_pantalla));
}

void InicializaArkanoPi(tipo_arkanoPi *p_arkanoPi) {
	ResetArkanoPi(p_arkanoPi);
	ActualizaPantalla(p_arkanoPi);
}

void ResetArkanoPi(tipo_arkanoPi *p_arkanoPi) {
	ReseteaPantalla((tipo_pantalla*)(p_arkanoPi->p_pantalla));
	InicializaLadrillos((tipo_pantalla*)(&(p_arkanoPi->ladrillos)));
	InicializaPelota((tipo_pelota*)(&(p_arkanoPi->pelota)));
	InicializaPala((tipo_pala*)(&(p_arkanoPi->pala)));
}

void CambiarDireccionPelota(tipo_pelota *p_pelota, enum t_direccion direccion) {
	if((direccion < 0)||(direccion > p_pelota->num_posibles_trayectorias)) {
		printf("[ERROR!!!!][direccion NO VALIDA!!!!][%d]", direccion);
		return;
	}
	else {
		p_pelota->trayectoria.xv = p_pelota->posibles_trayectorias[direccion].xv;
		p_pelota->trayectoria.yv = p_pelota->posibles_trayectorias[direccion].yv;
	}
}

void ActualizaPosicionPala(tipo_pala *p_pala, enum t_direccion direccion) {
	switch (direccion) {
		case DERECHA:
			// Dejamos que la pala rebase parcialmente el límite del area de juego
			if( p_pala->x + 1 + p_pala->ancho <= NUM_COLUMNAS_DISPLAY + 2 )
				p_pala->x = p_pala->x + 1;
			break;
		case IZQUIERDA:
			// Dejamos que la pala rebase parcialmente el límite del area de juego
			if( p_pala->x - 1 >= -2)
					p_pala->x = p_pala->x - 1;
			break;
		default:
			printf("[ERROR!!!!][direccion NO VALIDA!!!!][%d]", direccion);
			break;
	}
}

void ActualizaPosicionPelota (tipo_pelota *p_pelota) {
	p_pelota->x += p_pelota->trayectoria.xv;
	p_pelota->y += p_pelota->trayectoria.yv;
}

int CompruebaReboteLadrillo (tipo_arkanoPi *p_arkanoPi) {
	int p_posible_ladrillo_x = 0;
	int p_posible_ladrillo_y = 0;

	p_posible_ladrillo_x = p_arkanoPi->pelota.x + p_arkanoPi->pelota.trayectoria.xv;

	if ( ( p_posible_ladrillo_x < 0) || ( p_posible_ladrillo_x >= NUM_COLUMNAS_DISPLAY ) ) {
		printf("\n\nERROR GRAVE!!! p_posible_ladrillo_x = %d!!!\n\n", p_posible_ladrillo_x);
		fflush(stdout);
		exit(-1);
	}

	p_posible_ladrillo_y = p_arkanoPi->pelota.y + p_arkanoPi->pelota.trayectoria.yv;

	if ( ( p_posible_ladrillo_y < 0) || ( p_posible_ladrillo_y >= NUM_FILAS_DISPLAY ) ) {
		printf("\n\nERROR GRAVE!!! p_posible_ladrillo_y = %d!!!\n\n", p_posible_ladrillo_y);
		fflush(stdout);
	}

	if(p_arkanoPi->ladrillos.matriz[p_posible_ladrillo_y][p_posible_ladrillo_x] > 0 ) {
		// La pelota ha entrado en el area de ladrillos
		// y descontamos el numero de golpes que resta para destruir el ladrillo
		p_arkanoPi->ladrillos.matriz[p_posible_ladrillo_y][p_posible_ladrillo_x] = p_arkanoPi->ladrillos.matriz[p_posible_ladrillo_y][p_posible_ladrillo_x] - 1;

		return 1;
	}
	return 0;
}

int CompruebaReboteParedesVerticales (tipo_arkanoPi arkanoPi) {
	// Comprobamos si la nueva posicion de la pelota excede los limites de la pantalla
	if((arkanoPi.pelota.x + arkanoPi.pelota.trayectoria.xv >= NUM_COLUMNAS_DISPLAY) ||
		(arkanoPi.pelota.x + arkanoPi.pelota.trayectoria.xv < 0) ) {
		// La pelota rebota contra la pared derecha o izquierda
		return 1;
	}
	return 0;
}

int CompruebaReboteTecho (tipo_arkanoPi arkanoPi) {
	// Comprobamos si la nueva posicion de la pelota excede los limites de la pantalla
	if(arkanoPi.pelota.y + arkanoPi.pelota.trayectoria.yv < 0) {
		// La pelota rebota contra la pared derecha o izquierda
		return 1;
	}
	return 0;
}

int CompruebaRebotePala (tipo_arkanoPi arkanoPi) {
	if(arkanoPi.pelota.trayectoria.yv > 0) { // Esta condicion solo tiene sentido si la pelota va hacia abajo en la pantalla
		if ((arkanoPi.pelota.x + arkanoPi.pelota.trayectoria.xv >= arkanoPi.pala.x ) &&
			(arkanoPi.pelota.x + arkanoPi.pelota.trayectoria.xv < arkanoPi.pala.x + NUM_COLUMNAS_PALA)) {
				if ((arkanoPi.pelota.y + arkanoPi.pelota.trayectoria.yv >= arkanoPi.pala.y) &&
					(arkanoPi.pelota.y + arkanoPi.pelota.trayectoria.yv < arkanoPi.pala.y + NUM_FILAS_PALA)) {
					return 1;
				}
		}
	}
	return 0;
}

int CompruebaFallo (tipo_arkanoPi arkanoPi) {
	// Comprobamos si no hemos conseguido devolver la pelota
	if(arkanoPi.pelota.y + arkanoPi.pelota.trayectoria.yv >= NUM_FILAS_DISPLAY) {
		// Hemos fallado
		return 1;

	}
	return 0;
}

int CompruebaReboteSuelo (tipo_arkanoPi arkanoPi){
	if(arkanoPi.pelota.y + arkanoPi.pelota.trayectoria.yv == NUM_FILAS_DISPLAY){
		return 1;
	}
	return 0;
}

int CalculaLadrillosRestantes(tipo_pantalla *p_ladrillos) {
	int i=0, j=0;
	int numLadrillosRestantes;

	numLadrillosRestantes = 0;

	for(i=0;i<NUM_FILAS_DISPLAY;i++) {
		for(j=0;j<NUM_COLUMNAS_DISPLAY;j++) {
			if(p_ladrillos->matriz[i][j] != 0) {
				numLadrillosRestantes++;
			}
		}
	}

	return numLadrillosRestantes;
}

int CompruebaVida (tipo_arkanoPi arkanoPi){
	if(arkanoPi.vidas > 3){
		return 1;
	}

	return 0;
}

//------------------------------------------------------
// FUNCIONES DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

int CompruebaBotonPulsado (fsm_t* this) {
	int result = 0;

	// A completar por el alumno
	// ...

	piLock (SYSTEM_FLAGS_KEY);
	result = (flags & FLAG_BOTON);
	piUnlock (SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaMovimientoIzquierda(fsm_t* this) {
	int result = 0;

	// A completar por el alumno
	// ...

	piLock (SYSTEM_FLAGS_KEY);
	result = (flags & FLAG_MOV_IZQUIERDA);
	piUnlock (SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaMovimientoDerecha(fsm_t* this) {
	int result = 0;

	// A completar por el alumno
	// ...

	piLock (SYSTEM_FLAGS_KEY);
    result = (flags & FLAG_MOV_DERECHA);
	piUnlock (SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaTimeoutActualizacionJuego (fsm_t* this) {
	int result = 0;

	// A completar por el alumno
	// ...
	piLock (SYSTEM_FLAGS_KEY);
    result = (flags & FLAG_TIMER_JUEGO);
    piUnlock (SYSTEM_FLAGS_KEY);


	return result;
}

int CompruebaFinalJuego(fsm_t* this) {
	int result = 0;

	// A completar por el alumno
	// ...

	piLock (SYSTEM_FLAGS_KEY);
	result = (flags & FLAG_FIN_JUEGO);
	piUnlock (SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaDificultadFacil (fsm_t* this){
	int result = 0;

	piLock (SYSTEM_FLAGS_KEY);
	result = (flags & FLAG_DIF_FACIL);
	piUnlock (SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaDificultadMedia (fsm_t* this){
	int result = 0;

	piLock (SYSTEM_FLAGS_KEY);
	result = (flags & FLAG_DIF_MEDIA);
	piUnlock (SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaDificultadDificil (fsm_t* this){
	int result = 0;

	piLock (SYSTEM_FLAGS_KEY);
	result = (flags & FLAG_DIF_DIFICIL);
	piUnlock (SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaDificultadIncremental (fsm_t* this){
	int result = 0;

	piLock (SYSTEM_FLAGS_KEY);
	result = (flags & FLAG_DIF_INCREMENTA);
	piUnlock (SYSTEM_FLAGS_KEY);


	return result;
}

int CompruebaPause (fsm_t* this){
	int result = 0;

	piLock (SYSTEM_FLAGS_KEY);
	result = (flags & FLAG_PAUSE);
	piUnlock (SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaPlay (fsm_t* this){
	int result = 0;

	piLock (SYSTEM_FLAGS_KEY);
	result = (flags & FLAG_PLAY);
	piUnlock (SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaJuegoConVidas(fsm_t* this){
	int result = 0;

	piLock (SYSTEM_FLAGS_KEY);
	result = (flags & FLAG_CON_VIDAS);
	piUnlock (SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaJuegoSinVidas(fsm_t* this){
	int result = 0;

	piLock (SYSTEM_FLAGS_KEY);
	result = (flags & FLAG_SIN_VIDAS);
	piUnlock (SYSTEM_FLAGS_KEY);

	return result;
}

//------------------------------------------------------
// FUNCIONES DE ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

// void InicializaJuego (void): funcion encargada de llevar a cabo
// la oportuna inicialización de toda variable o estructura de datos
// que resulte necesaria para el desarrollo del juego.

void InicializaJuego(fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);


	// A completar por el alumno
	// ...

	piLock (SYSTEM_FLAGS_KEY);
	flags &= ~FLAG_BOTON;
	piUnlock (SYSTEM_FLAGS_KEY);

	InicializaArkanoPi(p_arkanoPi);

	piLock(STD_IO_BUFFER_KEY);
	PintaPantallaPorTerminal((tipo_pantalla *) (p_arkanoPi->p_pantalla));
	piUnlock(STD_IO_BUFFER_KEY);


	tmr_startms(p_arkanoPi->tmr_actualizacion_juego, p_arkanoPi->dificultad);

	//PseudowiringPiEnableDisplay(1);
}

void DificultadFacil(fsm_t* this){
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	piLock (SYSTEM_FLAGS_KEY);
	flags &= ~FLAG_DIF_FACIL;
	piUnlock (SYSTEM_FLAGS_KEY);

	p_arkanoPi->dificultad = DIFICULTAD_FACIL;

	piLock(STD_IO_BUFFER_KEY);
	printf("\n Has seleccionado la dificultad FÁCIL");
	piUnlock(STD_IO_BUFFER_KEY);

	//PintaInstrucciones();
	PintaMensajeVida();
}

void DificultadMedia(fsm_t* this){
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	piLock (SYSTEM_FLAGS_KEY);
	flags &= ~FLAG_DIF_MEDIA;
	piUnlock (SYSTEM_FLAGS_KEY);

	p_arkanoPi->dificultad = DIFICULTAD_MEDIA;

	piLock(STD_IO_BUFFER_KEY);
	printf("\n Has seleccionado la dificultad MEDIA");
	piUnlock(STD_IO_BUFFER_KEY);

	//PintaInstrucciones();
	PintaMensajeVida();
}

void DificultadDificil(fsm_t* this){
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	piLock (SYSTEM_FLAGS_KEY);
	flags &= ~FLAG_DIF_DIFICIL;
	piUnlock (SYSTEM_FLAGS_KEY);

	p_arkanoPi->dificultad = DIFICULTAD_DIFICIL;

	piLock(STD_IO_BUFFER_KEY);
	printf("\n Has seleccionado la dificultad DIFICIL");
	piUnlock(STD_IO_BUFFER_KEY);

	//PintaInstrucciones();
	PintaMensajeVida();;
}

void DificultadIncremental(fsm_t* this){
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	piLock (SYSTEM_FLAGS_KEY);
	flags &= ~FLAG_DIF_INCREMENTA;
	piUnlock (SYSTEM_FLAGS_KEY);

	p_arkanoPi->dificultad = DIFICULTAD_INCREMENTAL;

	p_arkanoPi->dificultad_incremental = 1;


	piLock(STD_IO_BUFFER_KEY);
	printf("\n Has seleccionado la dificultad INCREMENTAL");
	piUnlock(STD_IO_BUFFER_KEY);

	//PintaInstrucciones();
	PintaMensajeVida();
}

void PauseJuego(fsm_t* this){
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	piLock (SYSTEM_FLAGS_KEY);
	flags &= ~FLAG_PAUSE;
	piUnlock (SYSTEM_FLAGS_KEY);

	tmr_startms(p_arkanoPi->tmr_actualizacion_juego, 0);
}

void PlayJuego(fsm_t* this){
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	piLock (SYSTEM_FLAGS_KEY);
	flags &= ~FLAG_PLAY;
	piUnlock (SYSTEM_FLAGS_KEY);

	tmr_startms(p_arkanoPi->tmr_actualizacion_juego, p_arkanoPi->dificultad);
}

void JuegoConVidas(fsm_t* this){
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	piLock (SYSTEM_FLAGS_KEY);
	flags &= ~FLAG_CON_VIDAS;
	piUnlock (SYSTEM_FLAGS_KEY);

	p_arkanoPi->vidas = 0;

	PintaInstrucciones();
}

void JuegoSinVidas(fsm_t* this){
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	piLock (SYSTEM_FLAGS_KEY);
	flags &= ~FLAG_SIN_VIDAS;
	piUnlock (SYSTEM_FLAGS_KEY);

	p_arkanoPi->vidas = 3;

	PintaInstrucciones();

}

void ActualizaVida(fsm_t* this){
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);
	p_arkanoPi->vidas = p_arkanoPi->vidas + 1;
}


// void MuevePalaIzquierda (void): funcion encargada de ejecutar
// el movimiento hacia la izquierda contemplado para la pala.
// Debe garantizar la viabilidad del mismo mediante la comprobación
// de que la nueva posición correspondiente a la pala no suponga
// que ésta rebase o exceda los límites definidos para el área de juego
// (i.e. al menos uno de los leds que componen la raqueta debe permanecer
// visible durante todo el transcurso de la partida).

void MuevePalaIzquierda (fsm_t* this) {
	tipo_arkanoPi* p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	// A completar por el alumno
	// ...
	piLock(SYSTEM_FLAGS_KEY);
	flags &= (~FLAG_MOV_IZQUIERDA);
	piUnlock(SYSTEM_FLAGS_KEY);

	ActualizaPosicionPala((tipo_pala *)&(p_arkanoPi->pala), IZQUIERDA);

	piLock(MATRIX_KEY);
	ActualizaPantalla(p_arkanoPi);
	piUnlock(MATRIX_KEY);

	piLock(STD_IO_BUFFER_KEY);
	PintaPantallaPorTerminal((tipo_pantalla *)(p_arkanoPi->p_pantalla));
	piUnlock(STD_IO_BUFFER_KEY);


}

// void MuevePalaDerecha (void): función similar a la anterior
// encargada del movimiento hacia la derecha.

void MuevePalaDerecha (fsm_t* this) {
	tipo_arkanoPi* p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	// A completar por el alumno
	// ...
	piLock(SYSTEM_FLAGS_KEY);
	flags &= (~FLAG_MOV_DERECHA);
	piUnlock(SYSTEM_FLAGS_KEY);

	ActualizaPosicionPala((tipo_pala *)&(p_arkanoPi->pala), DERECHA);

	piLock(MATRIX_KEY);
	ActualizaPantalla(p_arkanoPi);
	piUnlock(MATRIX_KEY);

	piLock(STD_IO_BUFFER_KEY);
	PintaPantallaPorTerminal((tipo_pantalla *)(p_arkanoPi->p_pantalla));
	piUnlock(STD_IO_BUFFER_KEY);
}

// void ActualizarJuego (void): función encargada de actualizar la
// posición de la pelota conforme a la trayectoria definida para ésta.
// Para ello deberá identificar los posibles rebotes de la pelota para,
// en ese caso, modificar su correspondiente trayectoria (los rebotes
// detectados contra alguno de los ladrillos implicarán adicionalmente
// la eliminación del ladrillo). Del mismo modo, deberá también
// identificar las situaciones en las que se dé por finalizada la partida:
// bien porque el jugador no consiga devolver la pelota, y por tanto ésta
// rebase el límite inferior del área de juego, bien porque se agoten
// los ladrillos visibles en el área de juego.

void ActualizarJuego (fsm_t* this) {
	tipo_arkanoPi* p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	// A completar por el alumno
	// ...

	piLock (SYSTEM_FLAGS_KEY);
	flags &= (~FLAG_TIMER_JUEGO);
	flags &= (~FLAG_BOTON);
	piUnlock (SYSTEM_FLAGS_KEY);

	if(CompruebaReboteParedesVerticales((tipo_arkanoPi)*(p_arkanoPi))){
		p_arkanoPi->pelota.trayectoria.xv = -p_arkanoPi->pelota.trayectoria.xv;
	}

	if(CompruebaReboteTecho((tipo_arkanoPi)*(p_arkanoPi))){
		p_arkanoPi->pelota.trayectoria.yv = -p_arkanoPi->pelota.trayectoria.yv;
	}

	if(CompruebaReboteSuelo((tipo_arkanoPi)*(p_arkanoPi))){
		ActualizaVida(this);
		if(!CompruebaVida((tipo_arkanoPi)*(p_arkanoPi))){
			p_arkanoPi->pelota.trayectoria.yv = -p_arkanoPi->pelota.trayectoria.yv;
		}
	}

	if(CompruebaFallo((tipo_arkanoPi)*(p_arkanoPi))){
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_FIN_JUEGO;
			piUnlock(SYSTEM_FLAGS_KEY);
			return;

	} else if(CompruebaRebotePala((tipo_arkanoPi)*(p_arkanoPi))){
		switch(p_arkanoPi->pelota.x + p_arkanoPi->pelota.trayectoria.xv - p_arkanoPi->pala.x ){
			case 0:
				CambiarDireccionPelota(((tipo_pelota *)&(p_arkanoPi->pelota)), ARRIBA_IZQUIERDA);
			break;
			case 1:
				CambiarDireccionPelota(((tipo_pelota *)&(p_arkanoPi->pelota)), ARRIBA);
			break;
			case 2:
				CambiarDireccionPelota(((tipo_pelota *)&(p_arkanoPi->pelota)), ARRIBA_DERECHA);
			break;

			default:
			break;
		}
	}
	if(CompruebaReboteLadrillo(p_arkanoPi)){
		p_arkanoPi->pelota.trayectoria.yv = -p_arkanoPi->pelota.trayectoria.yv;
		p_arkanoPi->pelota.trayectoria.xv = -p_arkanoPi->pelota.trayectoria.xv;

		p_arkanoPi->puntos = (p_arkanoPi->puntos) + 1;
		digitalWrite(GPIO_ARDUINO_1, HIGH);

		if((p_arkanoPi->dificultad_incremental == 1) && ((p_arkanoPi->puntos % 4) == 0)){
				p_arkanoPi->dificultad = p_arkanoPi->dificultad/2;
		}

		if(CalculaLadrillosRestantes((tipo_pantalla *)&(p_arkanoPi->ladrillos)) <= 0){
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_FIN_JUEGO;
			piUnlock(SYSTEM_FLAGS_KEY);
			return;
		} else if (CompruebaReboteTecho((tipo_arkanoPi)*(p_arkanoPi))){
			p_arkanoPi->pelota.trayectoria.xv = -p_arkanoPi->pelota.trayectoria.xv;
			p_arkanoPi->pelota.trayectoria.yv = -p_arkanoPi->pelota.trayectoria.yv;
		} //else if(CompruebaReboteParedesVerticales((tipo_arkanoPi)*(p_arkanoPi))){
			//p_arkanoPi->pelota.trayectoria.xv = -p_arkanoPi->pelota.trayectoria.xv;
			//p_arkanoPi->pelota.trayectoria.yv = -p_arkanoPi->pelota.trayectoria.yv;
		//}
		digitalWrite(GPIO_ARDUINO_1, LOW);
	}


	//ActualizaPosicionPala((tipo_pala *)&(p_arkanoPi->pala), (p_arkanoPi->pelota.num_posibles_trayectorias));
	ActualizaPosicionPelota((tipo_pelota*)&(p_arkanoPi->pelota));



	piLock(MATRIX_KEY);
	ActualizaPantalla(p_arkanoPi);
	piUnlock(MATRIX_KEY);

	piLock(STD_IO_BUFFER_KEY);
	PintaPantallaPorTerminal((tipo_pantalla *) (p_arkanoPi->p_pantalla));
	piUnlock(STD_IO_BUFFER_KEY);

	tmr_startms(p_arkanoPi->tmr_actualizacion_juego, p_arkanoPi->dificultad);
}

// void FinalJuego (void): función encargada de mostrar en la ventana de
// terminal los mensajes necesarios para informar acerca del resultado del juego.

void FinalJuego (fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	// A completar por el alumno
	// ...

	piLock (SYSTEM_FLAGS_KEY);
	flags &= (~FLAG_FIN_JUEGO);
	piUnlock (SYSTEM_FLAGS_KEY);

	if(CalculaLadrillosRestantes((tipo_pantalla *)(p_arkanoPi->p_pantalla)) > 0){
		PintaCaraTriste(this);
		printf("\nFINAL DEL JUEGO\n");
		printf("¡Has perdido!\n");
		printf("Puntos: %i \n",(p_arkanoPi->puntos));
		printf("\nPulse 5 para seleccionar la dificultad o 1 para salir del juego\n");
	} else {
		PintaCaraContenta(this);
		printf("\n¡HAS GANADO!\n");
		printf("Pulse 5 dos veces para volver a comenzar\n");

	}



	//PseudowiringPiEnableDisplay(0);
}

//void ReseteaJuego (void): función encargada de llevar a cabo la
// reinicialización de cuantas variables o estructuras resulten
// necesarias para dar comienzo a una nueva partida.

void ReseteaJuego (fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	// A completar por el alumno
	// ...

	piLock (SYSTEM_FLAGS_KEY);
	flags &= (~FLAG_BOTON);
	piUnlock (SYSTEM_FLAGS_KEY);

	ReseteaPantalla((tipo_pantalla*)(p_arkanoPi->p_pantalla));
	ResetArkanoPi(p_arkanoPi);

	p_arkanoPi->puntos = 0;
	p_arkanoPi->vidas = 0;

	piLock(STD_IO_BUFFER_KEY);

	printf ("\nSeleccione el nivel de dificultad\n");
	printf ("Tecla 7 para dificultad fácil.\n");
	printf ("Tecla 8 para dificultad media.\n");
	printf ("Tecla 9 para dificultad dificil.\n");
	printf ("Tecla 0 para dificultad incremental.\n");

	piUnlock(STD_IO_BUFFER_KEY);
}

//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------

void tmr_actualizacion_juego_isr(union sigval value) {
	// A completar por el alumno
	// ...
	piLock (SYSTEM_FLAGS_KEY);
	flags |= FLAG_TIMER_JUEGO;
	piUnlock (SYSTEM_FLAGS_KEY);


}


