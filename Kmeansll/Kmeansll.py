"""
Kmeansll                -   k-means||
Allen Ramirez Ramirez   -   B25448

Comandos de ejecucion
mpiexec -n 2 python -m mpi4py Kmeansll.py -k 100 -c 10000 -m 100
mpiexec -n 2 python -m mpi4py Kmeansll.py -k 100 -c 100000 -m 100
"""

import sys, getopt
import numpy as np
import random
from mpi4py import MPI
from numpy import genfromtxt
from numpy  import array
from scipy.spatial import distance

comm = MPI.COMM_WORLD
pid = comm.rank
size = comm.size

def obt_valores_linea_comandos(argv):
    k_str = ""
    c_str = ""
    m_str = ""
	
    try:
        ## parsea la lista de parametros de consola
        ## en opts queda una lista de pares (opcion, valor)
        ## en args queda una lista de valores sin las opciones
        opts, args = getopt.getopt(argv,"h:k:c:m:",["H=","K=","C=","M="])
    except getopt.GetoptError:
        print ('TP2_B25448.py -k <valor de K> -c <valor de C> -m <valor de M>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print ('TP2_B25448.py -k <valor de K> -c <valor de C> -m <valor de M>')
            sys.exit()
        elif opt in ("-k", "--K"):
            k_str = arg
        elif opt in ("-c", "--C"):
            c_str = arg
        elif opt in ("-m", "--M"):
            m_str = arg

    return int(k_str), int(c_str), int(m_str)

def kmeansII(x_datos, k_agrupamientos, l_sobremuestreos, m_dimensionalidad):
    tiempo = 0.0
    tiempo_total = np.zeros(size)
    max_tiempo = 0.0
    minimo = 0.0
    fi_sumatoria = 0.0
    i_log = 0
    i_final = 5
    x_random = 0
    distancia_x_c = 0.0
    tam_real_X = len(x_datos)
    datos_lenght = len(x_datos)//size
    if(len(x_datos)%size != 0):
        datos_lenght += 1
    c_centroides = []
    c_temp = np.array([])
    c_totales = np.zeros(0)
    c_totales.astype(int)
    centroides_ktt = []
    indice = []
    c_inicial = np.array([])
    c_ktt = np.array([])
    es_centroide = np.zeros(tam_real_X)
    son_centroide = np.zeros(tam_real_X)
    psi_distancias = np.zeros(datos_lenght)
    grupos_K = np.zeros(datos_lenght)
    pesos_w_c_i = np.zeros(tam_real_X)
    c_centroides = array(c_centroides)
    c_totales = array(c_totales)
    c_centroides.astype(int)
    tam = 0
    sumatoria = 0
    cant_centroides = 0
    iter = 0
    conteo = 1
    cont_actual = 0
    id_max = 0
    peso = 0
    offset_X = pid*datos_lenght
        
    t = MPI.Wtime()
    if pid == 0:
        random.seed()
        x_random = random.randint(0, (tam_real_X)-1)
        c_centroides = np.append(c_centroides, x_random)
        es_centroide[x_random] = True
    comm.Barrier()
    x_random = comm.bcast((x_random), 0)
    comm.Barrier()
    
    c_inicial = np.append(c_inicial, x_random)
    c_inicial = c_inicial.astype(int)
    distancias = distance.cdist(x_datos[offset_X:offset_X+datos_lenght], x_datos[c_inicial], 'euclidean')
    psi_distancias = np.fromiter(map(min, distancias),dtype=np.float)
    fi_sumatoria = sum(psi_distancias)
    
    while(i_log < i_final or cant_centroides < k_agrupamientos):
        c_temp = np.array([])
        for x in range(datos_lenght):
            if(offset_X+x < tam_real_X):
                if(not es_centroide[offset_X+x]):
                    if(((l_sobremuestreos)*psi_distancias[x])/fi_sumatoria > random.uniform(0,1)):
                        es_centroide[offset_X+x] = True
                        fi_sumatoria -= psi_distancias[x]
                        c_temp = np.append(c_temp, offset_X+x)
        
        comm.Barrier()
        tam = np.array(comm.gather(len(c_temp), 0))
        if pid == 0:
            c_totales = np.zeros(sum(tam))
        comm.Gatherv(sendbuf=c_temp, recvbuf=(c_totales, tam), root=0)
        if pid == 0:
            c_centroides = np.concatenate((c_centroides, c_totales), axis=0)
        c_centroides = comm.bcast((c_centroides), 0)
        comm.Barrier()
        c_centroides = c_centroides.astype(int)
        
        comm.Reduce(es_centroide, son_centroide, op=MPI.LOR, root=0)
        es_centroide = comm.bcast((son_centroide), 0)
        comm.Barrier()
        
        fi_sumatoria = 0.0
        
        distancias = distance.cdist(x_datos[offset_X:offset_X+datos_lenght], x_datos[c_centroides], 'euclidean')
        psi_distancias = np.fromiter(map(min, distancias),dtype=np.float)
        grupos_K = np.fromiter(map(np.argmin, distancias),dtype=np.int)
        fi_sumatoria = sum(psi_distancias)
        if(i_log == (i_final-1)):
            for k in grupos_K:
                pesos_w_c_i[k] += 1

        comm.Barrier()
        fi_sumatoria = comm.reduce(fi_sumatoria, root=0)
        fi_sumatoria = comm.bcast((fi_sumatoria), 0)
        
        cant_centroides = len(c_centroides)
        i_log += 1
    #end while

    fi_sumatoria = 0.0
    
    tam_real = len(c_centroides)
    centroides_lenght = len(c_centroides)//size
    if(len(c_centroides)%size != 0):
        centroides_lenght += 1
    ktt_es_centroide = np.zeros(tam_real)
    son_centroide = np.zeros(tam_real)
    
    if pid == 0:
        random.seed()
        x_random = int(random.randint(0, (tam_real-1)))
        c_ktt = np.append(c_ktt, c_centroides[x_random])
        c_ktt = c_ktt.astype(int)
        ktt_es_centroide[x_random] = True
    (c_ktt,ktt_es_centroide) = comm.bcast((c_ktt,ktt_es_centroide), 0)
    comm.Barrier()
    
    psi_distancias = np.zeros(centroides_lenght)
    offset_C = pid*centroides_lenght
    while(len(c_ktt) < k_agrupamientos):
        distancias = distance.cdist(x_datos[c_centroides[offset_C:offset_C+centroides_lenght]], x_datos[c_ktt], 'euclidean')
        psi_distancias = np.fromiter(map(min, distancias),dtype=np.float)
        fi_sumatoria = sum(psi_distancias)

        comm.Barrier()
        fi_sumatoria = comm.reduce(fi_sumatoria, root=0)
        fi_sumatoria = comm.bcast((fi_sumatoria), 0)
        c_temp = np.array([])
        for c in range(centroides_lenght):
            cont_actual = 0
            if(offset_C+c < tam_real):
                if(not ktt_es_centroide[offset_C+c] and conteo < k_agrupamientos):
                    if(psi_distancias[c] / fi_sumatoria > random.uniform(0,1)):
                        if(conteo < k_agrupamientos):
                            ktt_es_centroide[offset_C+c] = True
                            c_temp = np.append(c_temp, c_centroides[offset_C+c])
                            fi_sumatoria -= psi_distancias[c]
                        cont_actual += 1
            comm.Barrier()
            cont_actual = comm.reduce(cont_actual, root=0)
            if pid == 0:
                conteo += cont_actual
            conteo = comm.bcast((conteo), 0)
        
        comm.Barrier()
        tam = np.array(comm.gather(len(c_temp), 0))
        if pid == 0:
            c_totales = np.zeros(sum(tam))
        comm.Gatherv(sendbuf=c_temp, recvbuf=(c_totales, tam), root=0)
        if pid == 0:
            c_ktt = np.concatenate((c_ktt, c_totales), axis=0)
        c_ktt = comm.bcast((c_ktt), 0)
        comm.Barrier()
        c_ktt = c_ktt.astype(int)
        
        comm.Reduce(ktt_es_centroide, son_centroide, op=MPI.LOR, root=0)
        ktt_es_centroide = comm.bcast((son_centroide), 0)
        
        comm.Barrier()
        fi_sumatoria = 0.0
    #Fin while

    if pid == 0:
        while(len(c_ktt) > k_agrupamientos):
            c_ktt = c_ktt[:-1]
    c_ktt = comm.bcast((c_ktt), 0)

    indices = np.zeros(tam_real)
    indices_ktt = np.zeros(tam_real)
    
    distancias = distance.cdist(x_datos[c_centroides[offset_C:offset_C+centroides_lenght]], x_datos[c_ktt], 'euclidean')
    grupos_K = np.fromiter(map(np.argmin, distancias),dtype=np.int)
    for k in range(len(grupos_K)):
        indices_ktt[offset_C+k] = grupos_K[k]

    comm.Barrier()
    comm.Reduce(indices_ktt, indices, op=MPI.SUM, root=0)
    indices_ktt = comm.bcast((indices), 0)
    pesos = np.zeros(tam_real_X)
    comm.Reduce(pesos_w_c_i, pesos, op=MPI.SUM, root=0)
    pesos_w_c_i = comm.bcast((pesos), 0)
    comm.Barrier()

    pesos = np.array([])
    ides = np.array([])
    for k in range(k_agrupamientos):
        id_max = c_ktt[k]
        peso = pesos_w_c_i[c_ktt[k]]
        for xk in range(centroides_lenght):
            if(offset_C+xk < tam_real):
                if(pesos_w_c_i[c_centroides[offset_C+xk]] > peso and indices_ktt[offset_C+xk] == c_ktt[k]):
                    id_max = c_centroides[offset_C+xk]
                    peso = pesos_w_c_i[id_max]
        
        comm.Barrier()
        pesos = comm.gather(pesos_w_c_i[id_max], root = 0)
        ides = comm.gather(id_max, root = 0)
        if pid == 0:
            id_max = np.argmax(pesos)
            id_max = ides[id_max]
        id_max = comm.bcast((id_max), 0)
        
        c_ktt[k] = id_max
        comm.Barrier()

    tiempo = MPI.Wtime() - t
    tiempo_total = comm.gather(tiempo, 0)
    if(pid == 0):
        max_tiempo = np.amax(tiempo_total)
    max_tiempo = comm.bcast((max_tiempo), 0)
    
    return c_ktt, max_tiempo
    
def kmeans(k_agrupamientos, x_datos, centroides, m_dimensionalidad, epsilon):
    tiempo = 0.0
    tiempo_total = np.zeros(size)
    max_tiempo = 0.0
    tam_real_X = len(x_datos)
    datos_lenght = len(x_datos)//size
    if(len(x_datos)%size != 0):
        datos_lenght += 1
    c_variables = np.zeros((k_agrupamientos,m_dimensionalidad))
    c_temp = np.zeros((k_agrupamientos,m_dimensionalidad))
    distancias = np.array([])
    minimos = np.zeros(datos_lenght)
    k_lenght = 0
    grupos_K = np.zeros(datos_lenght)
    costo_Anterior = 0.0
    costo_Actual = 0.0
    costo_dif = epsilon+1
    elem_Centroide = np.zeros(k_agrupamientos)
    elem_Centroide2 = np.zeros(k_agrupamientos)
    pesos = np.zeros(datos_lenght)
    fc = 0
    indice = 0
    distancia_x_c = 0.0
    sumatoria = 0.0
    minimo = 0.0
    t = 0.0
    division_K = k_agrupamientos//size
    if(k_agrupamientos%size != 0):
        division_K += 1
    sumatoria_X = 0.0
    conteo = 0
    vector_temp = np.zeros(m_dimensionalidad)
    offset_X = pid*datos_lenght
    offset_K = pid*division_K
    
    t = MPI.Wtime()
    for c in range(division_K):
        if(offset_K+c < k_agrupamientos):
            c_variables[offset_K+c] = x_datos[centroides[offset_K+c]]
    comm.Reduce(c_variables, c_temp, op=MPI.SUM, root=0)
    c_variables = comm.bcast((c_temp), 0)
    
    while(costo_dif > epsilon):
        costo_Anterior = costo_Actual
        costo_Actual = 0
        elem_Centroide = np.zeros(k_agrupamientos)
        
        distancias = distance.cdist(x_datos[offset_X:offset_X+datos_lenght], c_variables, 'euclidean')
        minimos = np.fromiter(map(min, distancias),dtype=np.float)
        grupos_K = np.fromiter(map(np.argmin, distancias),dtype=np.int)
        costo_Actual = sum(minimos**2)
        for k in grupos_K:
            elem_Centroide[k] += 1
        
        comm.Barrier()
        costo_Actual = comm.reduce(costo_Actual, root=0)
        costo_Actual = comm.bcast((costo_Actual), 0)
        comm.Reduce(elem_Centroide, elem_Centroide2, op=MPI.SUM, root=0)
        elem_Centroide = comm.bcast((elem_Centroide2), 0)
        comm.Barrier()

        c_variables = np.zeros((k_agrupamientos,m_dimensionalidad))
        for x in range(datos_lenght):
            if(offset_X+x < tam_real_X):
                grupo = grupos_K[x]
                c_variables[grupo] = np.add(c_variables[grupo], (x_datos[offset_X+x]/elem_Centroide[grupo]))

        comm.Barrier()
        comm.Reduce(c_variables, c_temp, op=MPI.SUM, root=0)
        c_variables = comm.bcast((c_temp), 0)

        if(costo_Anterior != 0):
            costo_dif = costo_Anterior - costo_Actual
        comm.Barrier()
    #while end
    
    tiempo = MPI.Wtime() - t
    tiempo_total = comm.gather(tiempo, 0)
    if(pid == 0):
        max_tiempo = np.amax(tiempo_total)
    max_tiempo = comm.bcast((max_tiempo), 0)

    return costo_Actual, max_tiempo, c_variables, grupos_K, elem_Centroide
	
def escribir_archivo(tiempo_total, costo, x_datos, c_variables, grupos_K, elem_Centroide, k_agrupamientos):
    tam_real_X = len(x_datos)
    datos_lenght = len(x_datos)//size
    if(len(x_datos)%size != 0):
        datos_lenght += 1
    offset_X = pid*datos_lenght
    
    g = ""
    p = ""
    for c in range(k_agrupamientos):
        if(pid == 0):
            g += np.array2string(c_variables[c], separator=', ')
            g += "\n"
        comm.Barrier()
        for x in range(datos_lenght):
            if(offset_X+x < tam_real_X):
                if(grupos_K[x] == c):
                    p += np.array2string(x_datos[offset_X+x], separator=', ')
                    p += "\n"
        comm.Barrier()
        p = comm.reduce(p, root=0)
        
        if(pid == 0):
            g += p
            g += str(int(elem_Centroide[c]))
            g += "\n"
            g += "\n"
        p = ""
    
    comm.Barrier()
    if pid == 0:
        with open("salida.csv", 'w') as arch:
            arch.write(g)
            arch.write("\n")
            arch.write("Tiempo pared: ")
            arch.write(str(tiempo_total))
            arch.write("       ")
            arch.write("Costo: ")
            arch.write(str(costo))
        arch.close()
    
    return
    
def main(argv):
    tiempo_total = 0.0
    k_agrupamientos = 0
    c_cantidad = 0
    m_dimensionalidad = 0
    l_sobremuestreos = 50
    epsilon = 1
    x_datos = []
    g = ""
    costo = 0.0
    c_variables = np.zeros((k_agrupamientos,m_dimensionalidad))
    elem_Centroide = np.zeros(k_agrupamientos)
    datos_lenght = len(x_datos)//size
    if(len(x_datos)%size != 0):
        datos_lenght += 1
    grupos_K = np.zeros(datos_lenght)

    if pid == 0:
        k_agrupamientos, c_cantidad, m_dimensionalidad = obt_valores_linea_comandos(argv)
    k_agrupamientos, c_cantidad, m_dimensionalidad = comm.bcast((k_agrupamientos,c_cantidad,m_dimensionalidad),0)

    if pid == 0:
        x_datos = genfromtxt('vectores_desordenados.csv', delimiter=',')
        assert len(x_datos) == c_cantidad,"Error de entrada: 'c' (cantidad) dada no coincide con el del archivo."
        l_sobremuestreos = k_agrupamientos//3
        if(l_sobremuestreos == 0):
            l_sobremuestreos = 1
    x_datos, l_sobremuestreos = comm.bcast((x_datos, l_sobremuestreos), 0)
    
    (centroides_ktt,tiempo) = kmeansII(x_datos, k_agrupamientos, l_sobremuestreos, m_dimensionalidad)
    tiempo_total += tiempo
    (costo, tiempo, c_variables, grupos_K, elem_Centroide) = kmeans(k_agrupamientos, x_datos, centroides_ktt, m_dimensionalidad, epsilon)
    tiempo_total += tiempo
    
    escribir_archivo(tiempo_total, costo, x_datos, c_variables, grupos_K, elem_Centroide, k_agrupamientos)
    
    return
    
if __name__ == "__main__": 	## implica que reading_argv está siendo ejecutado directamente y no por otro módulo
	main(sys.argv[1:])		## le pasa a main la lista de opciones, parametros obtenida del sistema



#mpiexec -n 2 python -m mpi4py B25448Ampliacion.py -k 100 -c 10000 -m 100
#mpiexec -n 2 python -m mpi4py B25448Ampliacion.py -k 100 -c 100000 -m 100