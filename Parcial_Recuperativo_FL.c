
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include <semaphore.h>

// ()___()
// (° x °)
// (u___u)
//  U   U

#define Cantidad_Cajas 4;

pthread_t hiloClientes[20];
pthread_t hiloFuncionario;

typedef struct atencion
{
    int Nombre;
    int Status;
    pthread_t Clientes;
}Cajas;

typedef struct afiliado
{
    pthread_t id;
    int Status;
}Clientes;

int Posicion=0,Cajas_Llenas=0,Clientess_Listos,Repetir;
Cajas caja[4];
Clientes cola[20];


sem_t semIngreso;
sem_t semCola;


void *cajaDisponible(void* arg)
{
    Cajas_Llenas=0;
    int p=0,b,Sumado=0;
    long id = (long) arg;
    int pos = (int) id;
    sem_wait(&semIngreso);

    for(p=0;p<4;p++)
    {
        if(cola[pos].Status==1)
        {
            if(caja[p].Status==1)
            {
                printf("Estimado cliente, actualmente la caja %d , se encuentra ocupada.\n",p);
                Cajas_Llenas++;
            }else
            {
                printf("Estimado cliente, actualmente la caja %d , se encuentra disponible\n",p);
                caja[p].Status = 1;
                caja[p].Clientes = cola[pos].id;
                printf("El Clientes %d ha ingresado actualmente a la caja %d.\n",pos,p);
                sleep(5);
                printf("El Clientes %d ha dejado  actualmente la caja %d\n",pos,p);
                Clientess_Listos--;
                caja[p].Status = 0;
                cola[pos].Status = 2;
                break;
            }
        }
    }
    if(Cajas_Llenas==4)
    {
        printf("En este momento las no hay cajas vacias disponibles. Clientes ha sido agregado a la cola.\n");
    }

    sem_post(&semIngreso);
}

void *solicitarCaja()
{
	pthread_t tid;
    int oo;
	tid = pthread_self();
    sem_wait(&semCola);
    for(oo=0;oo<Repetir;oo++)
    {
        if(cola[oo].Status==0)
        {
            cola[oo].Status=1;
            cola[oo].id=tid;
            break;
        }
    }
	printf("Estimado cliente: ha ingresado a la sucursal. Posicion en la cola: (%d/%d). ID: %ld\n\n",oo,oo,tid);
    Posicion++;
    sem_post(&semCola);
}


int main()
{
    long i;
    sem_init(&semIngreso, 0, 4);
    sem_init(&semCola,0,1);
    int confirmacion;
    for(int p = 0; p<4;p++)
    {
       caja[p].Nombre = p;
       caja[p].Status = 0;
    }
    for(int p = 0; p<20; p++)
    {
        cola[p].Status=0;
        cola[p].id=0;
    }
    printf("Estimado, inserte el numero de clientes a ingresar (Maximo de 20)\n");
    scanf("%d",&Repetir);
    Clientess_Listos = Repetir;
    for(i=0;i<Repetir;i++)
    {
        confirmacion = pthread_create(&hiloClientes[i], NULL, &solicitarCaja, NULL);
		if(confirmacion != 0){
			printf("Ha ocurrido un error: %d, error al lanzar el hilo\n", confirmacion);
        }
    }
    for(i=0;i<Repetir;i++)
    {
		pthread_join(hiloClientes[i], NULL);
    }
    do{
        for(i=0;i<Repetir;i++)
        {
            confirmacion = pthread_create(&hiloFuncionario, NULL, &cajaDisponible, (void*)i);
            if(confirmacion != 0)
            {
	            printf("Ha ocurrido un error: %d, error al lanzar el hilo\n", confirmacion);
                exit(-1);
            }
        }
        for(i=0;i<Repetir;i++)
        {
            pthread_join(hiloFuncionario, NULL);
        }
    }while(Clientess_Listos>1);
    return 0;
}

