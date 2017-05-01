/*
 ============================================================================
 Name        : kruskaka.c
 Author      : 
 Version     :
 Copyright   : a veces siento que
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#ifdef CACA_COMUN_LOG
#include <execinfo.h>
#endif

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#else
#include <time.h>
#include <stdarg.h>
#endif

#define CACA_COMUN_TAM_MAX_LINEA 200
#define CACA_LOG_MAX_TAM_CADENA 2000

#define CACA_COMUN_BUF_STATICO (char[1000] ) { '\0' }

#define BITCH_VECTOR_NUM_BITS (sizeof(bitch_vector) * 8)

#define CACA_COMUN_ASSERT_DUROTE 0
#define CACA_COMUN_ASSERT_SUAVECITO 1
#define CACA_COMUN_ASSERT_NIMADRES 2

typedef unsigned int natural;
typedef natural tipo_dato;

typedef long long bitch_vector;

typedef enum BOOLEANOS {
	falso = 0, verdadero
} bool;

/*
 #define CACA_COMUN_TIPO_ASSERT CACA_COMUN_ASSERT_SUAVECITO
 */
#define CACA_COMUN_TIPO_ASSERT CACA_COMUN_ASSERT_DUROTE

#define assert_timeout_dummy(condition) 0;
static inline void caca_log_debug_dummy(const char *format, ...) {
}

#if CACA_COMUN_TIPO_ASSERT == CACA_COMUN_ASSERT_DUROTE
#define assert_timeout(condition) assert(condition);
#endif
#if CACA_COMUN_TIPO_ASSERT == CACA_COMUN_ASSERT_SUAVECITO
#define assert_timeout(condition) if(!(condition)){sleep(10);abort();}
#endif
#if CACA_COMUN_TIPO_ASSERT == CACA_COMUN_ASSERT_NIMADRES
#define assert_timeout(condition) 0
#endif

#ifdef CACA_COMUN_LOG
#define caca_log_debug(formato, args...) \
		do \
		{ \
			size_t profundidad = 0; \
			void *array[CACA_LOG_MAX_TAM_CADENA]; \
 			profundidad = backtrace(array, CACA_LOG_MAX_TAM_CADENA); \
			caca_log_debug_func(formato,__FILE__, __func__, __LINE__,profundidad,##args); \
		} \
		while(0);
#else
#define caca_log_debug caca_log_debug_dummy
#endif

#define caca_comun_max(x,y) ((x) < (y) ? (y) : (x))
#define caca_comun_min(x,y) ((x) < (y) ? (x) : (y))

void caca_log_debug_func(const char *format, ...);

#define caca_comun_mapa_bitch_checa(bits, posicion, resultado) \
        __asm__ (\
                        "xor %%rdx,%%rdx\n"\
                        "movq %[bitch_posi],%%rax\n" \
                        "movq $64,%%r8\n"\
                        "divq %%r8\n"\
                        "mov $1,%[resul]\n"\
                        "mov %%rdx,%%rcx\n"\
                        "shlq %%cl,%[resul]\n"\
                        "and (%[vectors],%%rax,8),%[resul]\n"\
                        : [resul] "=b" (resultado)\
                        : [bitch_posi] "r" (posicion), [vectors] "r" (bits)\
            :"rax","rdx","rcx","r8")

static inline void caca_comun_mapa_bitch_asigna(bitch_vector *bits,
		unsigned long posicion) {
	int idx_arreglo = 0;
	int idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	bits[idx_arreglo] |= (bitch_vector) ((bitch_vector) 1 << idx_registro);

}

static inline void caca_comun_mapa_bitch_limpia(bitch_vector *bits,
		unsigned long posicion) {
	int idx_arreglo = 0;
	int idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	bits[idx_arreglo] &= (bitch_vector) ~((bitch_vector) 1 << idx_registro);

}

void caca_comun_current_utc_time(struct timespec *ts) {

#ifdef __MACH__
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts->tv_sec = mts.tv_sec;
	ts->tv_nsec = mts.tv_nsec;
#else
#ifdef CACA_COMUN_LOG
	clock_gettime(CLOCK_REALTIME, ts);
#endif
#endif

}

void caca_comun_timestamp(char *stime) {
	time_t ltime;
	struct tm result;
	long ms;
	struct timespec spec;
	char parte_milisecundos[50];

	ltime = time(NULL );

#ifndef ONLINE_JUDGE
	localtime_r(&ltime, &result);
	asctime_r(&result, stime);
#endif

	*(stime + strlen(stime) - 1) = ' ';

	caca_comun_current_utc_time(&spec);
#ifndef ONLINE_JUDGE
	ms = round(spec.tv_nsec / 1.0e3);
#endif
	sprintf(parte_milisecundos, "%ld", ms);
	strcat(stime, parte_milisecundos);
}
void caca_log_debug_func(const char *format, ...) {

	va_list arg;
	va_list arg2;
	const char *PEDAZO_TIMESTAMP_HEADER = "tiempo: %s; ";
	const char *HEADER =
			"archivo: %s; funcion: %s; linea %d; nivel: %zd caca 8====D ";
	char formato[CACA_LOG_MAX_TAM_CADENA + sizeof(HEADER)
			+ sizeof(PEDAZO_TIMESTAMP_HEADER)] = { '\0' };
	char pedazo_timestamp[sizeof(PEDAZO_TIMESTAMP_HEADER) + 100] = { '\0' };
	char cadena_timestamp[100] = { '\0' };

	caca_comun_timestamp(cadena_timestamp);
	sprintf(pedazo_timestamp, PEDAZO_TIMESTAMP_HEADER, cadena_timestamp);

	strcpy(formato, pedazo_timestamp);
	strcat(formato, HEADER);
	strcat(formato, format);
	strcat(formato, "\n");

	va_start(arg, format);
	va_copy(arg2, arg);
	vprintf(formato, arg2);
	va_end(arg2);
	va_end(arg);
// XXX: http://stackoverflow.com/questions/1716296/why-does-printf-not-flush-after-the-call-unless-a-newline-is-in-the-format-strin
	setbuf(stdout, NULL );
}

#ifdef CACA_COMUN_LOG
static char *caca_comun_arreglo_a_cadena(tipo_dato *arreglo, int tam_arreglo,
		char *buffer) {
	int i;
	char *ap_buffer = NULL;
	int characteres_escritos = 0;
#ifdef ONLINE_JUDGE
	return NULL;
#endif

	memset(buffer, 0, 100);
	ap_buffer = buffer;

	for (i = 0; i < tam_arreglo; i++) {
		characteres_escritos += sprintf(ap_buffer + characteres_escritos,
				"%u", *(arreglo + i));
		if (i < tam_arreglo - 1) {
			*(ap_buffer + characteres_escritos++) = ',';
		}
	}
	*(ap_buffer + characteres_escritos) = '\0';
	return ap_buffer;
}

static char *caca_comun_arreglo_a_cadena_natural(natural *arreglo,
		natural tam_arreglo, char *buffer) {
	int i;
	char *ap_buffer = NULL;
	int characteres_escritos = 0;
#ifdef ONLINE_JUDGE
	return NULL;
#endif

	memset(buffer, 0, 100);
	ap_buffer = buffer;

	for (i = 0; i < tam_arreglo; i++) {
		characteres_escritos += sprintf(ap_buffer + characteres_escritos, "%2u",
				*(arreglo + i));
		if (i < tam_arreglo - 1) {
			*(ap_buffer + characteres_escritos++) = ',';
		}
	}
	*(ap_buffer + characteres_escritos) = '\0';
	return ap_buffer;
}
#endif
void caca_comun_strreplace(char s[], char chr, char repl_chr) {
	int i = 0;
	while (s[i] != '\0') {
		if (s[i] == chr) {
			s[i] = repl_chr;
		}
		i++;
	}
}

static inline int caca_comun_lee_matrix_long_stdin(tipo_dato *matrix,
		int *num_filas, int *num_columnas, int num_max_filas,
		int num_max_columnas) {
	int indice_filas = 0;
	int indice_columnas = 0;
	tipo_dato numero = 0;
	char *siguiente_cadena_numero = NULL;
	char *cadena_numero_actual = NULL;
	char *linea = NULL;

	linea = calloc(CACA_COMUN_TAM_MAX_LINEA, sizeof(char));

	while (indice_filas < num_max_filas
			&& fgets(linea, CACA_COMUN_TAM_MAX_LINEA, stdin)) {
		indice_columnas = 0;
		cadena_numero_actual = linea;
		caca_comun_strreplace(linea, '\n', '\0');
		if (!strlen(linea)) {
			caca_log_debug("weird, linea vacia\n");
			continue;
		}
		for (siguiente_cadena_numero = linea;; siguiente_cadena_numero =
				cadena_numero_actual) {
			caca_log_debug("el numero raw %s\n", linea);
			numero = strtol(siguiente_cadena_numero, &cadena_numero_actual, 10);
			if (cadena_numero_actual == siguiente_cadena_numero) {
				break;
			}
			*(matrix + indice_filas * num_max_columnas + indice_columnas) =
					numero;
			caca_log_debug("en col %d, fil %d, el valor %lu\n", indice_columnas,
					indice_filas, numero);
			indice_columnas++;
			caca_log_debug("las columnas son %d\n", indice_columnas);
		}
		if (num_columnas) {
			num_columnas[indice_filas] = indice_columnas;
		}
		indice_filas++;
		caca_log_debug("las filas son %d, con clos %d\n", indice_filas,
				indice_columnas);
	}

	*num_filas = indice_filas;
	free(linea);
	return 0;
}

typedef struct kruskaca_sub_caca {
	natural rango;
	natural padre;
} kruskaca_sub_caca;

typedef struct kruskaca_vergatice {
	tipo_dato nodo_origen;
	tipo_dato nodo_destino;
	natural peso;
} kruskaca_vergatice;

static inline tipo_dato kruskaca_encuentra(tipo_dato llave,
		kruskaca_sub_caca *sub_cacas) {
	tipo_dato llave_padre = 0;
	tipo_dato llave_actual = 0;
	kruskaca_sub_caca *sub_caca_inicial = NULL;
	kruskaca_sub_caca *sub_caca_actual = NULL;

	llave_actual = llave;
	sub_caca_actual = sub_caca_inicial = sub_cacas + llave_actual;

	while (sub_caca_actual->padre != llave_actual) {
		llave_actual = sub_caca_actual->padre;
		sub_caca_actual = sub_cacas + llave_actual;
	}

	llave_padre = llave_actual;
	sub_caca_inicial->padre = llave_padre;

	return llave_padre;
}

static inline void kruskaca_union(tipo_dato llave1, tipo_dato llave2,
		kruskaca_sub_caca *sub_cacas) {
	tipo_dato padre1 = 0;
	tipo_dato padre2 = 0;
	kruskaca_sub_caca *sub_caca_1 = NULL;
	kruskaca_sub_caca *sub_caca_2 = NULL;

	padre1 = kruskaca_encuentra(llave1, sub_cacas);
	padre2 = kruskaca_encuentra(llave2, sub_cacas);

	sub_caca_1 = sub_cacas + padre1;
	sub_caca_2 = sub_cacas + padre2;

	if (sub_caca_1->rango < sub_caca_2->rango) {
		sub_caca_1->padre = padre2;
	} else {
		sub_caca_2->padre = padre1;
		if (sub_caca_1->rango > sub_caca_2->rango) {
		} else {
			sub_caca_1->rango++;
		}
	}
}

int kruskaca_ordenar_por_peso(const void *a, const void *b) {
	kruskaca_vergatice *va = (kruskaca_vergatice *) a;
	kruskaca_vergatice *vb = (kruskaca_vergatice *) b;

	return va->peso > vb->peso;
}

static inline kruskaca_vergatice *kruskaca_core(kruskaca_vergatice *vertices,
		kruskaca_sub_caca *sub_cacas, natural num_vertices,
		natural num_sub_cacas, kruskaca_vergatice *vertices_res) {

	natural num_vertices_res = 0;

	qsort(vertices, num_vertices, sizeof(kruskaca_vergatice),
			kruskaca_ordenar_por_peso);

	caca_log_debug("vergatices ordenados");

	for (int i = 0; i < num_sub_cacas; i++) {
		kruskaca_sub_caca *sub_caca_actual = sub_cacas + i;
		sub_caca_actual->padre = i;
		sub_caca_actual->rango = 0;
		caca_log_debug("inicializada sub caca %d", i);
	}

	for (int i = 0; i < num_vertices; i++) {
		kruskaca_vergatice *vert_act = vertices + i;
		tipo_dato padre_fuente = 0;
		tipo_dato padre_destino = 0;
		tipo_dato fuente = vert_act->nodo_origen;
		tipo_dato destino = vert_act->nodo_destino;
		natural peso = vert_act->peso;

		caca_log_debug("viendo vertice %d-%d peso %d", fuente, destino, peso);

		if (num_vertices_res >= (num_sub_cacas - 1)) {
			caca_log_debug("pero q mierda, ia ay %d veritces en el res",
					num_vertices_res);
			break;
		}

		padre_fuente = kruskaca_encuentra(fuente, sub_cacas);
		padre_destino = kruskaca_encuentra(destino, sub_cacas);

		caca_log_debug("el padre fuente %d padre dest %d", padre_fuente,
				padre_destino);

		if (padre_fuente != padre_destino) {

			caca_log_debug("uniendo %d con %d", padre_fuente, padre_destino);
			kruskaca_union(padre_fuente, padre_destino, sub_cacas);
			vertices_res[num_vertices_res++] = *vert_act;
		} else {
			caca_log_debug("ignorando olimpicamente verga %d-%d", fuente,
					destino);
		}
	}
	return vertices_res;
}

void kruskaca_main() {
	natural num_vertices = 0;
	natural num_nodos = 0;
	long long suma = 0;
	kruskaca_vergatice *vertices = NULL;
	kruskaca_sub_caca *sub_cacas = NULL;
	kruskaca_vergatice *vertices_res = NULL;

	scanf("%d %d", &num_nodos, &num_vertices);

	caca_log_debug("num nodos %d num verts %d", num_nodos, num_vertices);

	vertices = calloc(num_vertices, sizeof(kruskaca_vergatice));
	sub_cacas = calloc(num_nodos + 1, sizeof(kruskaca_sub_caca));
	vertices_res = calloc(num_nodos, sizeof(kruskaca_vergatice));

	for (int i = 0; i < num_vertices; i++) {
		tipo_dato fuente = 0;
		tipo_dato destino = 0;
		natural peso = 0;
		kruskaca_vergatice *vertice_act = vertices + i;

		scanf("%d %d %d", &fuente, &destino, &peso);

		vertice_act->nodo_origen = fuente;
		vertice_act->nodo_destino = destino;
		vertice_act->peso = peso;
	}

	vertices_res = kruskaca_core(vertices, sub_cacas, num_vertices, num_nodos,
			vertices_res);

	for (int i = 0; i < (num_nodos - 1); i++) {
		suma += vertices_res[i].peso;
	}

	caca_log_debug("la syma de vergas es %lld", suma);
	printf("%lld\n", suma);

	free(vertices);
	free(sub_cacas);
	free(vertices_res);
}

int main(void) {
	kruskaca_main();
	return EXIT_SUCCESS;
}
