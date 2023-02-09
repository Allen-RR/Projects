/* Allen Ramirez Ramirez - B25448
Trabajo de investigacion
Simulated Annealing */

#include <iostream>
#include <ostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>

using namespace std;

/*Variables globales*/
vector<int> mochila;
vector<int> subconjunto1; //Subconjuntos actuales. Se guarda la solucion actual
vector<int> subconjunto2;
vector<int> subconjunto3;
long long int valorSub1 = 0; //Valores actuales de cada subconjunto actual
long long int valorSub2 = 0;
long long int valorSub3 = 0;
vector<int> nuevoSubC1; //Subconjuntos nuevos. Se guarda la solucion nueva con la que se esta trabajando
vector<int> nuevoSubC2;
vector<int> nuevoSubC3;
long long int nuevoValor1 = 0; //Valores nuevos de cada subconjunto nuevo
long long int nuevoValor2 = 0;
long long int nuevoValor3 = 0;

//bool cero = false; //borrador

/*Lee los numeros desde el archivo "numeros.txt" que está en la misma carpeta y los divide aleatoriamente en los suconjuntos*/
void leerNumeros() {
	int numero = 0;
	ifstream myfile("numeros.txt"); //Nombre del archivo que contiene la lista de numeros a utilizar (misma carpeta)
	if (myfile.is_open())
	{
		//Agregar los numeros a un conjunto base y a un subconjunto al azar
		while (myfile >> numero) {
			mochila.push_back(numero);
			int subconjunto = (rand() % 3) + 1;
			switch (subconjunto)
			{
			case 1:
				subconjunto1.push_back(numero);
				valorSub1 += numero;
				break;
			case 2:
				subconjunto2.push_back(numero);
				valorSub2 += numero;
				break;
			case 3:
				subconjunto3.push_back(numero);
				valorSub3 += numero;
				break;
			default:
				break;
			}
		}
		myfile.close();
	}
	else {
		cerr << "Error. No se pudo abrir el archivo." << endl;
	}
	//Crear tambien los nuevos subconjuntos como copias
	nuevoSubC1 = subconjunto1; //Nuevos subconjuntos
	nuevoSubC2 = subconjunto2;
	nuevoSubC3 = subconjunto3;
	nuevoValor1 = valorSub1; //Nuevos valores
	nuevoValor2 = valorSub2;
	nuevoValor3 = valorSub3;
}

/*Calcula el puntaje de los subconjuntos actuales, usando f(s1, s2, s3) = (s1 - s2)^2 + (s1 - s3)^2 + (s2 - s3)^2 */
long long int calcularPuntaje() {
	long long int solucion = (
		(valorSub1 - valorSub2) * (valorSub1 - valorSub2) +
		(valorSub1 - valorSub3) * (valorSub1 - valorSub3) +
		(valorSub2 - valorSub3) * (valorSub2 - valorSub3));
	return solucion;
}

/*Calcula el puntaje de los subconjuntos nuevos, usando f(s1, s2, s3) = (s1 - s2)^2 + (s1 - s3)^2 + (s2 - s3)^2 */
long long int calcularPuntajeNuevo() {
	long long int solucionNueva = (
		(nuevoValor1 - nuevoValor2) * (nuevoValor1 - nuevoValor2) +
		(nuevoValor1 - nuevoValor3) * (nuevoValor1 - nuevoValor3) +
		(nuevoValor2 - nuevoValor3) * (nuevoValor2 - nuevoValor3));
	return solucionNueva;
}

/*Busca el vecino de la solucion con la que se esta trabajando*/
void cambiarConjuntos() {
	int cambiar = 0; //Valor a cambiar de subconjunto
	int fuente = 0; //Subconjunto del que sale el valor
	int destino = 0; //Subconjunto que recibe el valor
	bool cambiado = false; //Bandera que indica si se realizo el cambio

	while (!cambiado) { //Hasta que se realice el cambio
		fuente = (rand() % 3) + 1;
		destino = (rand() % 2) + 1;
		//Mover el valor desde el subconjunto fuente al subconjunto destino
		switch (fuente)
		{
		case 1:
			if (nuevoSubC1.size() != 1) {
				random_shuffle(nuevoSubC1.begin(), nuevoSubC1.end());
				cambiar = nuevoSubC1.back();
				nuevoSubC1.pop_back();
				nuevoValor1 -= cambiar;
				if (destino == 1) {
					nuevoSubC2.push_back(cambiar);
					nuevoValor2 += cambiar;
				}
				else {
					nuevoSubC3.push_back(cambiar);
					nuevoValor3 += cambiar;
				}
				cambiado = true;
			}
			break;
		case 2:
			if (nuevoSubC2.size() != 1) {
				random_shuffle(nuevoSubC2.begin(), nuevoSubC2.end());
				cambiar = nuevoSubC2.back();
				nuevoSubC2.pop_back();
				nuevoValor2 -= cambiar;
				if (destino == 1) {
					nuevoSubC1.push_back(cambiar);
					nuevoValor1 += cambiar;
				}
				else {
					nuevoSubC3.push_back(cambiar);
					nuevoValor3 += cambiar;
				}
				cambiado = true;
			}
			break;
		case 3:
			if (nuevoSubC3.size() != 1) {
				random_shuffle(nuevoSubC3.begin(), nuevoSubC3.end());
				cambiar = nuevoSubC3.back();
				nuevoSubC3.pop_back();
				nuevoValor3 -= cambiar;
				if (destino == 1) {
					nuevoSubC1.push_back(cambiar);
					nuevoValor1 += cambiar;
				}
				else {
					nuevoSubC2.push_back(cambiar);
					nuevoValor2 += cambiar;
				}
				cambiado = true;
			}
			break;
		default:
			cout << "Error." << endl;
			break;
		}
	}
}

/*Calcula la probabilidad para decidir si tomar nuevas soluciones*/
double calcularProbabilidad(long long int actualSolucion, long long int nuevaSolucion, long double temperatura) {
	long double metropolis = 0.0; //Metropolis: e^((Solucion actual menos solucion nueva) entre la temperatura)
	if (nuevaSolucion < actualSolucion) { //Si la nueva solucion es mejor, retorna 1 para aceptarla
		metropolis = 1.0;
	}
	else { //Si la nueva solucion es peor, se calcula la probabilidad a utilizar para intentar aceptarla
		metropolis = (long double)exp((long double)(((long double)(actualSolucion - nuevaSolucion)) / temperatura));
	}
	//if (!cero && metropolis == 0) { //borrador
		//cout << endl << "-------Probabilidad: " << metropolis << " Temperatura: " << temperatura << "-------" << endl; //borrador
		//cero = true; //borrador
	//} //borrador
	return metropolis;
}

/*Imprime datos de prueba para monitorear el avance y ver los resultados obtenidos*/
void imprimir(long long int actualSolucion, long long int nuevaSolucion, long double temperatura,
	int iter, long long int mejorSolucion) {
	long long int diferencia = actualSolucion - nuevaSolucion;
	long double division = diferencia / temperatura;
	long double metropolis = (nuevaSolucion < actualSolucion) ? 1 : exp(division);
	cout << "Iteracion:       " << iter << endl;
	cout << "Mejor  solucion: " << mejorSolucion << endl;
	cout << "Actual solucion: " << actualSolucion << endl;
	cout << "Nueva  solucion: " << nuevaSolucion << endl;
	cout << "Temp actual:     " << temperatura << endl;
	cout << "Diferencia :     " << diferencia << endl;
	cout << "Division   :     " << division << endl;
	cout << "Metropolis :     " << metropolis << endl;
	cout << endl;
	/*
	cout << "Metropolis:      " << (double)exp((double)((actualSolucion - nuevaSolucion) / temperatura)) << endl;
	cout << "Nuevo subconjunto 1:" << endl;
	for (std::vector<int>::const_iterator i = nuevoSubC1.begin(); i != nuevoSubC1.end(); ++i) {
		std::cout << *i << ' ';
	}
	cout << endl;
	cout << "Subconjunto 1:" << endl;
	for (std::vector<int>::const_iterator i = subconjunto1.begin(); i != subconjunto1.end(); ++i) {
		std::cout << *i << ' ';
	}
	cout << endl;
	cout << "Nuevo subconjunto 2:" << endl;
	for (std::vector<int>::const_iterator i = nuevoSubC2.begin(); i != nuevoSubC2.end(); ++i) {
		std::cout << *i << ' ';
	}
	cout << endl;
	cout << "Subconjunto 2:" << endl;
	for (std::vector<int>::const_iterator i = subconjunto2.begin(); i != subconjunto2.end(); ++i) {
		std::cout << *i << ' ';
	}
	cout << endl;
	cout << "Nuevo subconjunto 3:" << endl;
	for (std::vector<int>::const_iterator i = nuevoSubC3.begin(); i != nuevoSubC3.end(); ++i) {
		std::cout << *i << ' ';
	}
	cout << endl;
	cout << "Subconjunto 3:" << endl;
	for (std::vector<int>::const_iterator i = subconjunto3.begin(); i != subconjunto3.end(); ++i) {
		std::cout << *i << ' ';
	}
	cout << endl;
	cout << endl;
	*/
}

/*Metodo principal. Realiza el algoritmo base de Simulated Annealing*/
int main() {
	srand(time(NULL)); //Semilla para numeros random
	int iteraciones = 1; //Contador de iteraciones
	int mejorIteracion = 0; //Iteracion en donde se encontro la mejor solucion
	long long int mejorSolucion = 0; //Mejor solucion encontrada
	long long int actualSolucion = 0; //Solucion actual utilizada
	long long int nuevaSolucion = 0; //Nueva solucion al modificar la actual
	long long int tempInicial = 333777333777333; //Temperatura a utilizar. Resultados varian segun este valor
	//long long int tempInicial = 1000000000000; //Temperatura para metodo alternativo
	//long long int tempInicial = 5000000000000; //borrador
	long double temperatura = (long double)tempInicial; //Temperatura que se modifica segun la iteracion
	long long int mejorValor1 = 0; //Valor de los subconjuntos de la mejor solucion encontrada
	long long int mejorValor2 = 0;
	long long int mejorValor3 = 0;

	leerNumeros(); //Lee los numeros desde el archivo y los divide aleatoriamente en los suconjuntos*/
	actualSolucion = calcularPuntaje(); //Calcular el puntaje de la solucion actual
	mejorSolucion = actualSolucion; //Iniciar la mejor solucion encontrada
	mejorValor1 = valorSub1; //Iniciar los valores de la mejor solucion encontrada
	mejorValor2 = valorSub2;
	mejorValor3 = valorSub3;

	while (iteraciones <= 1000000) { //Numero de iteraciones para el programa
		nuevoSubC1 = subconjunto1; //Reiniciar los nuevos subconjuntos
		nuevoSubC2 = subconjunto2;
		nuevoSubC3 = subconjunto3;
		nuevoValor1 = valorSub1; //Reiniciar los valores de los nuevos subconjuntos
		nuevoValor2 = valorSub2;
		nuevoValor3 = valorSub3;
		cambiarConjuntos(); //Realizar un cambio minimo y aleatorio a la nueva solucion
		nuevaSolucion = calcularPuntajeNuevo(); //Calcular el puntaje de la nueva solucion

		if (iteraciones % 1000 == 0 || iteraciones == 1) { //Imprimir cada 1000 iteraciones
			imprimir(actualSolucion, nuevaSolucion, temperatura, iteraciones, mejorSolucion); //Imprimir algunos datos
		}

		if (calcularProbabilidad(actualSolucion, nuevaSolucion, temperatura) > (float)rand() / RAND_MAX) {
			//Si la solucion nueva es mejor o el valor metropolis es mayor que el numero random, se toma la nueva solucion
			actualSolucion = nuevaSolucion; //Actualizar solucion actual
			subconjunto1 = nuevoSubC1; //Actualizar los conjuntos actuales
			subconjunto2 = nuevoSubC2;
			subconjunto3 = nuevoSubC3;
			valorSub1 = nuevoValor1; //Actualizar los valores de los conjuntos actuales
			valorSub2 = nuevoValor2;
			valorSub3 = nuevoValor3;
		}

		if (actualSolucion < mejorSolucion) { //Si la solucion actual es la mejor encontrada hasta el momento
			mejorSolucion = actualSolucion; //Actualizar la mejor solucion
			mejorIteracion = iteraciones; //Actualizar la iteracion de la mejor solucion
			mejorValor1 = valorSub1; //Actualizar los valores de la mejor solucion
			mejorValor2 = valorSub2;
			mejorValor3 = valorSub3;
		}
		iteraciones++; //Aumentar las iteraciones
		temperatura = (long double)((long double)tempInicial / (long double)iteraciones); //Actualizar la temperatura (fast)
		//temperatura = (long double)((long double)tempInicial / (long double)sqrt(2 + iteraciones)); //Metodo alternativo de actualizar
		//temperatura -= 1; //borrador
	}

	//Imprimir resultados y algunos datos
	cout << "\nNumero de soluciones candidatas exploradas: " << iteraciones - 1 << endl;
	cout << "Mejor solucion encontrada: " << mejorSolucion << endl;
	cout << "En iteracion: " << mejorIteracion << endl;
	cout << "\nTemperatura utilizada: " << tempInicial << endl;
	cout << "Mejor subconjunto 1: Cantidad de valores: " << subconjunto1.size() << " Valor o puntaje: " << mejorValor1 << endl;
	cout << "Mejor subconjunto 2: Cantidad de valores: " << subconjunto2.size() << " Valor o puntaje: " << mejorValor2 << endl;
	cout << "Mejor subconjunto 3: Cantidad de valores: " << subconjunto3.size() << " Valor o puntaje: " << mejorValor3 << endl;
	//if (cero) cout << "Cero encontrado.\n" << endl; //borrador

	return 0;
}
