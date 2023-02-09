/*Examen Parcial 2, parte programada
Allen Ramírez Ramírez, B25448*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*Tipo partícula para trabajar las 5 particulas*/
struct Particula {
	double posicionX;
	double posicionY;
	double valor;
	double velocidadX;
	double velocidadY;
	double mejorX;
	double mejorY;
	double mejorValor;
};

/*Número de particulas*/
int numPart = 5;
struct Particula particulas[5];

/*Inicializa las posiciones de las particulas*/
void inicializarParticulas() {
	for (int i = 0; i < numPart; i++) {
		particulas[i].posicionX = (1000.0 - -1000.0) * ((double)rand() / (double)RAND_MAX) + -1000.0;
		particulas[i].posicionY = (1000.0 - -1000.0) * ((double)rand() / (double)RAND_MAX) + -1000.0;
	}
}

/*Calcula las velocidades de las particulas*/
void calcularVelocity(double w, double c1, double c2, double posicionXG, double posicionYG) {
	for (int i = 0; i < numPart; i++) {
		double r1 = (double)rand() / (double)RAND_MAX;
		double r2 = (double)rand() / (double)RAND_MAX;
		particulas[i].velocidadX = w * particulas[i].velocidadX + c1 * r1 * (particulas[i].mejorX - particulas[i].posicionX) + c2 * r2 * (posicionXG - particulas[i].posicionX);
		particulas[i].velocidadY = w * particulas[i].velocidadY + c1 * r1 * (particulas[i].mejorY - particulas[i].posicionY) + c2 * r2 * (posicionYG - particulas[i].posicionY);
	}
}

/*Calcula el valor f(x,y) o valor fitness*/
double calcularFitness(double x, double y) {
	double fit = (sin((x) / (180) - 4)) / ((x * x) / (100000) + 1) + (x * x) / (500000) + (cos((y) / (100) - 1)) / ((y * y) / (1000000) + 1) + (y * y) / (500000);
	return fit;
}

/*Imprime los valores en cada iteracion*/
imprimir(int iter, double posXG, double posYG, double valG) {
	printf("iter=%d: G = (%.5f, %.5f), f(G)= %.7f\n", iter, posXG, posYG, valG);
	printf("x1= (%.5f, %.5f), f(x1)= %.7f\n", particulas[0].posicionX, particulas[0].posicionY, particulas[0].valor);
	printf("x2= (%.5f, %.5f), f(x2)= %.7f\n", particulas[1].posicionX, particulas[1].posicionY, particulas[1].valor);
	printf("x3= (%.5f, %.5f), f(x3)= %.7f\n", particulas[2].posicionX, particulas[2].posicionY, particulas[2].valor);
	printf("x4= (%.5f, %.5f), f(x4)= %.7f\n", particulas[3].posicionX, particulas[3].posicionY, particulas[3].valor);
	printf("x5= (%.5f, %.5f), f(x5)= %.7f\n\n", particulas[4].posicionX, particulas[4].posicionY, particulas[4].valor);
}

/*Metodo principal*/
int main() {
	int iteracion = 1;
	int permanencia = 0;
	double acumulado = 0.0;
	double w = 0.3;
	double c1 = 0.8;
	double c2 = 1.5;
	double posicionXG = 0.0;
	double posicionYG = 0.0;
	double valorG = 9999999.0;
	srand(1);

	inicializarParticulas();
	calcularVelocity(w, c1, c2, posicionXG, posicionYG);

	while (iteracion <= 10000 && permanencia <= 50) {
		for (int i = 0; i < numPart; i++) {
			particulas[i].valor = calcularFitness(particulas[i].posicionX, particulas[i].posicionY);
			if (particulas[i].valor < particulas[i].mejorValor) {
				particulas[i].mejorX = particulas[i].posicionX;
				particulas[i].mejorY = particulas[i].posicionY;
				particulas[i].mejorValor = particulas[i].valor;
			}
			if (particulas[i].valor < valorG) {
				posicionXG = particulas[i].posicionX;
				posicionYG = particulas[i].posicionY;
				acumulado += (valorG - particulas[i].valor);
				//printf("Sumando: %f, %f, %f", valorG, particulas[i].valor, acumulado);
				valorG = particulas[i].valor;
				if (acumulado > 0.001) {
					permanencia = 0;
					acumulado = 0.0;
				}
			}
			calcularVelocity(w, c1, c2, posicionXG, posicionYG);
			//printf("\n");
			//printf("Particula: %d, Vel: %.7f, %.7f, Acumulado: %f\n", i, particulas[i].velocidadX, particulas[i].velocidadY, acumulado);
			//printf("\n");
			particulas[i].posicionX += particulas[i].velocidadX;
			particulas[i].posicionY += particulas[i].velocidadY;
			while (particulas[i].posicionX<-1000|| particulas[i].posicionX>1000|| particulas[i].posicionY<-1000|| particulas[i].posicionY>1000) {
				if ((particulas[i].posicionX) < -1000) {
					particulas[i].posicionX += ((particulas[i].posicionX + 1000) * 2);
				}
				else {
					if ((particulas[i].posicionX) > 1000) {
						particulas[i].posicionX -= ((particulas[i].posicionX - 1000) * 2);
					}
				}
				if ((particulas[i].posicionY) < -1000) {
					particulas[i].posicionY += ((particulas[i].posicionY + 1000) * 2);
				}
				else {
					if ((particulas[i].posicionY) > 1000) {
						particulas[i].posicionY -= ((particulas[i].posicionY - 1000) * 2);
					}
				}
			}
		}
		imprimir(iteracion, posicionXG, posicionYG, valorG);
		permanencia++;
		iteracion++;
	}
	printf("Solucion final: G = (%.5f, %.5f), f(G)= %.7f\n", posicionXG, posicionYG, valorG);

	return 0;
}
