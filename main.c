#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Map.h"
#include "list.h"
#include <stdbool.h>


typedef struct Pokedex Pokedex;
typedef struct pokemon pokemon;

struct Pokedex{
  char nombre[20];
  int existencia;
  int totalTIPOS; //Almacena el total de Tipos que posee cada Pokemon registrado
  char** tipo;
  char previa[20];
  char posterior[20];
  int num_pokedex;
  char region[20];
};

struct pokemon{
  int id;
  char nombre[20];
  int pc;
  int ps;
  int sexo;
};

typedef struct {
  char name[20];
  char country[20];
  int existencia;
}buscar;


int lower_than_int(void * key1, void * key2) {
    if(*(int*)key1 < *(int*)key2) return 1;
    return 0;
}

int is_equal_int(void * key1, void * key2) {
    if(*(int*)key1 == *(int*)key2) return 1;
    return 0;
}

int is_equal_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2)==0) return 1;
    return 0;
}

//Prototipos
void archivos(Map *, Map *, List *);
void agregarEvolucion(Map*, Pokedex*);
void atrapar(Map*, Map*, List *);
void evolucionar(Map*, Map*);
void buscarPOKEMON_PorTipo(Map*, Map*);
void buscarPOKEMON_nombre(Map *, List *);
void buscarPOKEDEX_nombre(Map*, List *);
void mostrarPOKEDEX(Map*);
void mostrarALMACENAMIENTO(Map *);
void liberarPOKEMON(Map *, Map *);
void buscarPOKEMON_region(Map*, List *);


//FUNCIONES
void cleanNUM(char *);
void cleanCHAR(char *);
int conversion(char *);
int numPOKEDEX(Map *);
int obtenerID(Map *);
int compararEnteros(const void*, const void*);
void menu();


int main(){


  //nombreMapa_CLAVE (nomenclatura)...
  Map * Almacen_ID = createMap(is_equal_int);
  Map * pokedex_NOMBRE = createMap(is_equal_string);
  List * datoUNICO = create_list();

  //Se ordena el ID
  setSortFunction(Almacen_ID, lower_than_int);
  int num;
  menu();
  scanf("%i", &num);
  
  while(num != 0)
  {
    switch(num)
    {
      case 1: archivos(Almacen_ID, pokedex_NOMBRE, datoUNICO); break;
      case 2: atrapar(Almacen_ID, pokedex_NOMBRE, datoUNICO); break;
      case 3: evolucionar(Almacen_ID, pokedex_NOMBRE); break;
      case 4: buscarPOKEMON_PorTipo(Almacen_ID, pokedex_NOMBRE); break;
      case 5: buscarPOKEMON_nombre(Almacen_ID, datoUNICO); break;
      case 6: buscarPOKEDEX_nombre(pokedex_NOMBRE, datoUNICO); break;
      case 7: mostrarPOKEDEX(pokedex_NOMBRE); break;
      case 8: mostrarALMACENAMIENTO(Almacen_ID); break;
      case 9: liberarPOKEMON(Almacen_ID, pokedex_NOMBRE); break;
      case 10: buscarPOKEMON_region(pokedex_NOMBRE, datoUNICO); break;
      default: printf("opción invalida\n"); break;
    }
    menu();
    scanf("%i", &num);
  }
  printf("-----------------\n");
  printf("Hasta la proxima!\n");
  printf("-----------------\n");

  return 0;
}

//OPCION 1: Importar / Exportar archivos...
void archivos(Map * Almacen_ID, Map * pokedex_NOMBRE, List * datoUNICO)
{
  /*
  Procedimiento para acceder el archivo y la comprobacion de su estado...
  */
  char csv[1000];

  printf("\nIngrese nombre del archivo\n");
  scanf("%s", csv);
  FILE *archivo = fopen(csv, "r");
  
  if(archivo == NULL){
    printf("\nError al abrir el archivo\n");
    exit(EXIT_FAILURE);
  }
  
  pokemon*  datos; //Corresponde al pokemon almacenado
  Pokedex* informacion; //Corresponde la informacion de tal pokemon
  buscar* L; //Corresponde los datos requeridos en otras funciones (nombre, existencia y region)

  
  char linea[150];
  int i, j ,k, cont;
  bool nuevo;
  
  //Se escaneo linea por linea del archivo
  while( fscanf(archivo, "%149[^\n]s", linea) != EOF)
  {
    
    fgetc(archivo);
    
    //Se evita la primera linea del archivo, que muestra el orden de los datos
    if(isdigit(linea[0]))
    {
      //El cursor del archivo...
      i = 0;

      datos = malloc(sizeof(pokemon));
      informacion = malloc(sizeof(Pokedex));
      L = malloc(sizeof(buscar));
      
      //Variables auxiliares...
      char numeros[6];
      char cadenaCaracteres[30];
      nuevo = true;

      //Leer ID...
      cleanNUM(numeros);
      for(i = 0; linea[i] != ','; i++)
        numeros[i] = linea[i];

      numeros[i + 1] = '\0';
      datos->id = conversion(numeros);
      
      i++;
      
      
      //Nombre
      cleanCHAR(cadenaCaracteres);
      for(k = 0; linea[i] != ',' ;i++ , k++)
        cadenaCaracteres[k] = linea[i];

      cadenaCaracteres[k] = '\0';
      strcpy(datos->nombre, cadenaCaracteres);
      
      //En base al nombre, se pregunta si esta registrado el pokemon
      if(searchMap(pokedex_NOMBRE, cadenaCaracteres) == NULL)
      {
        //Se agrega la informacion para la Pokedex
        strcpy(informacion->nombre, cadenaCaracteres);
        strcpy(L->name, cadenaCaracteres);

        informacion->existencia = 1;
        L->existencia = 1;
      }
      else
      {
        //En otro caso solo aumenta la existencia
        nuevo = false;
        Pokedex * dato = firstMap(pokedex_NOMBRE);

        while(strcmp(cadenaCaracteres,dato->nombre) != 0)
          dato = nextMap(pokedex_NOMBRE);

        dato->existencia++;
      }
      
      i++;
      
      //Tipos
      cleanCHAR(cadenaCaracteres);

      //Se pregunta si al leer los tipos empieza por ese caracter
      if(linea[i] == '"')
      {
        i++; 
        cont = -1;

        //Se agregan los tipos que escanea (mas de 1) 
        while(linea[i] != '"')
        { 
          cont++;

          //Se reserva la memoria para los tipos, el primero consiste el total de tipos y el segundo el nombre del tipo
          informacion->tipo = (char**) realloc(informacion->tipo, (cont + 1) * sizeof(char*));
          informacion->tipo[cont] = (char *) malloc(10 * sizeof(char));
                   
          for(j = 0; linea[i] != ',' && linea[i] != '"'; i++ , j++)
           cadenaCaracteres[j] = linea[i];

          cadenaCaracteres[j] = '\0';

          if(linea[i] == ',')
            i+=2;
        
          strcpy(informacion->tipo[cont], cadenaCaracteres);
          cleanCHAR(cadenaCaracteres);
        }
        i++;
        informacion->totalTIPOS = cont + 1;
      }
      else
      {
        //Se guarda solamente un tipo...
        informacion->tipo = malloc(1 * sizeof(char));
        informacion->tipo[0] = malloc(10 * sizeof(char));

        for(j = 0; linea[i] != ','; i++, j++)
          cadenaCaracteres[j] = linea[i];

        cadenaCaracteres[j] = '\0';
        

        strcpy(informacion->tipo[0], cadenaCaracteres);
        informacion->totalTIPOS = 1;
      }

      i++;

      //Puntos de Combate...
      cleanNUM(numeros);
      for(k = 0; linea[i] != ',' ; i++, k++)
        numeros[k] = linea[i];

      numeros[k + 1] = '\0';
      datos->pc = conversion(numeros);
      
      

      i++;
      //Puntos de Salud...
      cleanNUM(numeros);
      for(k = 0; linea[i] != ',' ; i++, k++)
        numeros[k] = linea[i];

      numeros[k + 1] = '\0';
      datos->ps = conversion(numeros);
      i++;
      

      //Sexo...
      cleanCHAR(cadenaCaracteres);
      for(j = 0; linea[i] != ','; i++, j++)
        cadenaCaracteres[j] = linea[i];

      cadenaCaracteres[j] = '\0';
      //Se guarda como int el sexo de la siguiente forma: 0 = HEMBRA, 1 = MACHO y 2 = NO TIENE
      if(strcmp(cadenaCaracteres, "Hembra ") == 0)
        datos->sexo = 0;

      if(strcmp(cadenaCaracteres, "Macho ") == 0)
        datos->sexo = 1;
      
      if(strcmp(cadenaCaracteres, "No tiene ") == 0)
        datos->sexo = 2;
        
      i++;

      
        //Evolucion Previa...
      cleanCHAR(cadenaCaracteres);
      for(j = 0; linea[i] != ','; i++, j++)
        cadenaCaracteres[j] = linea[i];

      cadenaCaracteres[j] = '\0';
      strcpy(informacion->previa,cadenaCaracteres);
      i++;
      
        //Evolucion Posterior...
      cleanCHAR(cadenaCaracteres);
      for(j = 0; linea[i] != ','; i++, j++)
        cadenaCaracteres[j] = linea[i];

      cadenaCaracteres[j] = '\0';
      strcpy(informacion->posterior,cadenaCaracteres);
        i++;

        //Numero pokedex...
      cleanNUM(numeros);
      for(k = 0; linea[i] != ',' ; i++, k++)
        numeros[k] = linea[i];

      numeros[k + 1] = '\0';
      informacion->num_pokedex = conversion(numeros);
      
      
      i++;

        //Region...
      cleanCHAR(cadenaCaracteres);
      for(j = 0; linea[i] != '\0'; i++, j++)
        cadenaCaracteres[j] = linea[i];

      cadenaCaracteres[j] = '\0';
      
      strcpy(informacion->region,cadenaCaracteres);
      strcpy(L->country, cadenaCaracteres);
      
      
      insertMap(Almacen_ID, &datos->id, datos);

      //Asegura de que el Pokemon repetido con los mismos datos no se inserte nuevamente
      if(nuevo)
      {
        insertMap(pokedex_NOMBRE, informacion->nombre, informacion);
        push_back(datoUNICO, L);
      }
    }
  }
  printf("\n------------------------\n");
  printf("Archivo leido con exito!");
  printf("\n------------------------\n");
}

//Limpia la string de dato numerico para almacenar otro dato a continacion
void cleanNUM(char * numeros)
{
  for(int i = 0; i <= 4; i++)
    numeros[i] = ' ';
}

//Limpia la string de letras para almacenar otro dato a continacion
void cleanCHAR(char * cadenaCaracteres)
{
  for(int i = 0; i <= 29; i++)
    cadenaCaracteres[i] = ' ';
}

//Convierte los datos numericos escritos en "Char" a dato totalmente numerico
int conversion(char * numeros)
{
  int i;
  int numero, decimas, aux;
  int potencia = -1;
  int suma = 0;
  
  //Cuento cuantos numeros ocupa el string para obtener su potencia...
  for(i = 1; numeros[i] != '\0'; i++)
      potencia++;

  aux = potencia;
  
  //En caso de ser 0 de potencia solo retorna un digito
  if(potencia == 0)
  {
    numero = numeros[0] - 48;
    return numero;
  }
  else
  {
    //Transformo cada numero del string 
    for(int k = 0; k < i - 1; k++)
    {
      decimas = 1;
      numero = numeros[k] - 48;
      
      //Aplico la potencia de 10 con respecto a la posicion del string
      for(;aux > 0; aux--)
      {
        decimas *= 10;
      }

      //Se le agrega el resultado a una sumatoria y se actualiza la potencia a evaluar
      numero *= decimas;
      suma += numero;
      potencia--;
      aux = potencia;
      
    }
    return suma;
  }
}


//OPCION 2: Atrapar pokemon | LISTA
void atrapar(Map* Almacen_ID, Map* pokedex_NOMBRE, List* datoUNICO)
{
  pokemon* nuevo_pokemon = malloc(sizeof(pokemon));

  char cadenaCaracteres[20];
  int numeros;
  

  printf("\nIngrese el nombre del pokemon: \n");
  scanf("%19s", cadenaCaracteres);
  strcpy(nuevo_pokemon->nombre, cadenaCaracteres);

  //En caso de que el Pokemon no este registrado en la Pokedex
  if(searchMap(pokedex_NOMBRE, cadenaCaracteres) == NULL)
  {
    char aux_tipos[30];
    Pokedex* nuevo_pokedex = malloc(sizeof(Pokedex));
    buscar * nuevo = malloc(sizeof(buscar));

    strcpy(nuevo_pokedex->nombre, cadenaCaracteres);
    strcpy(nuevo->name, cadenaCaracteres);

    nuevo_pokedex->existencia = 1;
    nuevo->existencia = 1;


    //Se le exige al usuario que complete la informacion que se requiere
    printf("\nIngrese la evolucion PREVIA de este pokemon:\n");
    getchar();
    scanf("%19[^\n]s", cadenaCaracteres);
    
    strcpy(nuevo_pokedex->previa, cadenaCaracteres);

    printf("\nAhora su evolucion POSTERIOR:\n");
    getchar();
    scanf("%19[^\n]s", cadenaCaracteres);
    
    
    strcpy(nuevo_pokedex->posterior, cadenaCaracteres);

    printf("\nIngrese la region:\n");
    getchar();
    scanf("%19[^\n]", cadenaCaracteres);
    
    
    strcpy(nuevo_pokedex->region, cadenaCaracteres);
    strcpy(nuevo->country, cadenaCaracteres);
    

    printf("\nIngrese Tipo(s): \n");
    getchar();
    scanf("%29[^\n]s", aux_tipos);
    
    
    int cont = -1;
    int j ,i = 0;
    
    while(aux_tipos[i] != '\0'){
      cont++;
      
      nuevo_pokedex->tipo = (char**) realloc(nuevo_pokedex->tipo, (cont + 1) * sizeof(char ));
      nuevo_pokedex->tipo[cont] = malloc(10 * sizeof(char ));
      
      j=0;
      while(1){

        
        if(aux_tipos[i] == ',' || aux_tipos[i] == '\0' || aux_tipos[i] == ' '){
          i++;
          break;
        }
        cadenaCaracteres[j] = aux_tipos[i];
        j++;
        i++;
        
      }

      cadenaCaracteres[j] = '\0';

      if(aux_tipos[i] == ',' || aux_tipos[i] == ' '){
        i++;
        
      }
      nuevo_pokedex->totalTIPOS = cont + 1;
      
      strcpy(nuevo_pokedex->tipo[cont], cadenaCaracteres);
      
      
    }

    nuevo_pokedex->num_pokedex = numPOKEDEX(pokedex_NOMBRE);
    printf("\n--------------------------------\n");
    printf("Pokemon Registrado en la POKEDEX\n");
    printf("--------------------------------\n\n");
    insertMap(pokedex_NOMBRE, nuevo_pokedex->nombre, nuevo_pokedex);
    push_back(datoUNICO, nuevo);
  }
  else
  {
    //Si el Pokemon estaba registado se incrementa una unidad a su existencia
    Pokedex* aux = firstMap(pokedex_NOMBRE);
    while(strcmp(cadenaCaracteres, aux->nombre) != 0)
      aux = nextMap(pokedex_NOMBRE);

    aux->existencia++;

    buscar* list = first(datoUNICO);
    while(strcmp(cadenaCaracteres, list->name) != 0)
      list = next(datoUNICO);

    list->existencia++;
  }

  //Se exige informacion de combate
  printf("\nIngrese sus Puntos de COMBATE: \n");
  scanf("%i", &numeros);
  nuevo_pokemon->pc = numeros;


  printf("\nIngrese sus Puntos de SALUD: \n");
  scanf("%i", &numeros);
  nuevo_pokemon->ps = numeros;


  printf("\nIngrese el sexo según corresponda:\n");
  printf("0 = Hembra \n1 = Macho \n2 = No tiene\n");

  scanf("%i", &numeros);
  while(numeros > 2 || numeros < 0)
  {
    printf("El numero que ingreso no corresponde a ninguno que se indicó.\n");
    scanf("%i", &numeros);
  }
  
  nuevo_pokemon->sexo = numeros;
  //printf("aaaaaaaaaaaa\n");
  nuevo_pokemon->id = obtenerID(Almacen_ID);
  
  insertMap(Almacen_ID, &nuevo_pokemon->id, nuevo_pokemon);
  printf("\n---------------------------------\n");
  printf("¡¡Has atrapado un nuevo Pokémon!!\n");
  printf("---------------------------------\n");
}

//Se asegura que el Numero de la Pokedex sea uno nuevo y uno que este ocupado
int numPOKEDEX(Map * pokedex_NOMBRE)
{
  int num;
  printf("\nIngrese NUEVO numero de la Pokedex: \nNUMERO = ");
  scanf("%i", &num);

  Pokedex * aux = firstMap(pokedex_NOMBRE);
  while(aux != NULL)
  {
    //Si el numero que ingreso esta ocupado, se reinicia esta funcion
    if(aux->num_pokedex == num)
    {
      printf("\nEl numero %i ya se encuentra ocupado, por favor ingrese otro...\n", num);
      return numPOKEDEX(pokedex_NOMBRE);
    }
    aux = nextMap(pokedex_NOMBRE);
  }
  return num;
}

//Se obtiene el ID del nuevo Pokemon a partir del ultimo
int obtenerID(Map * Almacen_ID)
{
  pokemon * aux = firstMap(Almacen_ID);
  int cont = 1;
  if(aux == NULL)
    return 1;

  while(nextMap(Almacen_ID) != NULL)
    cont++;
    aux = nextMap(Almacen_ID);

  return cont + 1;
  
}

void agregarEvolucion(Map* pokedex_NOMBRE,  Pokedex* x){
  Pokedex * p = malloc(sizeof(Pokedex));
  strcpy(p->nombre, x->posterior);
  
  p->existencia++;
  strcpy(p->previa, x->nombre);
  strcpy(p->posterior, "No tiene");

  strcpy(p->region, x->region);

  p->totalTIPOS = x->totalTIPOS;
  p->tipo = malloc(p->totalTIPOS * sizeof(char *));
  for(int i = 0; i < p->totalTIPOS; i++)
  {
    p->tipo[i] = malloc(10 * sizeof(char));
    strcpy(p->tipo[i], x->tipo[i]);
  }

  p->num_pokedex = numPOKEDEX(pokedex_NOMBRE);
  insertMap(pokedex_NOMBRE, p->nombre, p);
}
  

//OPCION 3:  Evolucionar pokemon | LISTA (idea: comparar los pc y los ps anteriores con los nuevos)

void evolucionar(Map* Almacen_ID, Map* pokedex_NOMBRE){
  pokemon * p = firstMap(Almacen_ID);
  Pokedex * x = firstMap(pokedex_NOMBRE);
  int evolucion_id;
  bool check = false;
  printf("Ingrese ID de pokémon a evolucionar: ");
  scanf("%d", &evolucion_id);
  printf("\n");

  while(p != NULL)
  {
    //Busca el ID en el almacenamiento

    if(evolucion_id == p->id)
    {
      //Busca si tal pokemon en la Pokedex
      while(x!= NULL)
      {
        if(strcmp(p->nombre, x->nombre) == 0)
        {
          if(check == false)
            check = true;

          //Comprueba si el Pokemon posee una evolucion
          if(strcmp(x->posterior, "No tiene") == 0)
          {
            printf("Este Pokemon se encuentra en su ultima version\n");
            break;
          }
          
          //Se hace las modificacion manteniendo el ID y compara el anterior con el actual
          p->pc = p->pc * 1.5;
          p->ps = p->ps * 1.25;
          strcpy(p->nombre, x->posterior);
          char name[30];
          strcpy(name, x->posterior); 
          printf("|ID | NOMBRE      | PC  | PS  |\n");
          printf("|%-3i| %-12s| %-4i| %-4i|\n", p->id, p->nombre, p->pc, p->ps);
          
         
          x->existencia--;
          
          printf("\n--------------------\n");
          printf("Pokemon Evolucionado\n");
          printf("--------------------\n");

          if(searchMap(pokedex_NOMBRE, name) == NULL) 
            agregarEvolucion(pokedex_NOMBRE, x);
          else
          {
            
            char aux[30];
            strcpy(aux, x->nombre);

            x = firstMap(pokedex_NOMBRE);
            while(strcmp(x->nombre, aux) == 0)
              x = nextMap(pokedex_NOMBRE);

            x->existencia++;
          }

          break;
        
        }
        x = nextMap(pokedex_NOMBRE);
      }
    }
    p = nextMap(Almacen_ID);
  }
  if(check == false)
  {
    printf("----------------------------------------------");
    printf("\nEl Pokemon con ese ID no existe o fue liberado\n");
    printf("----------------------------------------------");
  }
  printf("\n\n");
}

// OPCION 4: BUSCAR POKEMON POR TIPO | Listo

void buscarPOKEMON_PorTipo(Map* Almacen_ID, Map* pokedex_NOMBRE)
{
  char Tipo[20];
  bool check = false;
  getchar();
  printf("Ingrese el Tipo del pokemon\n");
  Pokedex * x = firstMap(pokedex_NOMBRE);
  
  scanf("%19[^\n]s", Tipo);
  printf("\n");
 
  int k, total;
  while(x != NULL)
  {
    //Se busca el Tipo en el Pokemon de la Pokedex registado
    total = x->totalTIPOS;
    
    for(k = 0; k < total ; k++)
    {
      
      
      if(strcmp(x->tipo[k], Tipo) == 0)
      {
        //Si hay al menos un Pokemon del tipo ingresado se imprime el encabezado de la informacion...
        if(check == false)
        {
          printf("\nINFORMACION:\n|ID | NOMBRE      | PC  | PS  |\n");
          check = true;
        }
        
        pokemon * aux = firstMap(Almacen_ID);
        
        //Empieza a buscar el Pokemon asociado al nombre que encontro la Pokedex...
        while(aux != NULL)
        {
          if(strcmp(aux->nombre, x->nombre) == 0){
            
            printf("|%-3i| %-12s| %-4i| %-4i|\n", aux->id, aux->nombre, aux->pc, aux->ps);
            
          }
          aux = nextMap(Almacen_ID);
        }
      }
    }
    
    x = nextMap(pokedex_NOMBRE);
  }
  //Imprime si no hay tal coincidencia
  if(check == false)
    printf("No hay registros de Pokémon para mostrar\n");
}


//OPCION 5: Buscar pokemons por nombre(ALMACENAMIENTO) | LISTA
void buscarPOKEMON_nombre(Map * Almacen_ID, List * datoUNICO)
{
  char nombre[20];
  buscar * p = first(datoUNICO);
  getchar();
  printf("Ingrese el nombre del pokemon\n");
  scanf("%s", nombre);
  
  
  
  while(p != NULL)
  {
    //Pregunta por el nombre que ingreso
    if(strcmp(p->name,nombre) == 0)
    {
      //Pregunta por la existencia
      if(p->existencia > 0)
      {
        //Se imprime la informacion del Pokemon basado en el nombre
        printf("\nINFORMACION:\n|ID | NOMBRE      | PC  | PS  |\n");
        pokemon * aux = firstMap(Almacen_ID);
        while(aux != NULL)
        {
          
          if(strcmp(aux->nombre, nombre) == 0)
          {
            printf("|%-3i| %-12s| %-4i| %-4i|\n", aux->id, aux->nombre, aux->pc, aux->ps);
          }
          aux = nextMap(Almacen_ID);
        }
        
        break;

      }
      else
      {
        printf("No tienes ese pokemon en tu almacenamiento\n\n");
        break;
      }
    }
    p = next(datoUNICO);
  }
  //Recorrio toda la lista y no lo encontro
  if(p == NULL)
   printf("No existe tal pokemon o no hay registros en la pokedex\n\n");
  
}

// OPCION 6: MOSTRAR LA INFO DE LA POKEDEX POR NOMBRE | LISTA

void buscarPOKEDEX_nombre(Map* pokedex_nombre , List * datoUNICO){
  char nombre[20];
  bool check = false;
  buscar * x = first(datoUNICO);

  getchar();
  printf("Ingrese el nombre del pokemon\n");
  scanf("%s", nombre);
  
  while(x != NULL)
  {
    //Verifica si esta el Pokemon con el nombre ingresado
    if(strcmp(x->name,nombre) == 0)
    {
      //Confirma el Pokemon con el nombre ingresado
      if(check == false)
        check = true;

        Pokedex * aux = firstMap(pokedex_nombre);
        while(aux != NULL)
        {
          
          if(strcmp(aux->nombre, nombre) == 0)
          {
            printf("\nNombre       |cant| Previo         |Posterior   |Num|Region | Tipo(s)\n");
            printf("--------------------------------------------------------------------------\n");
            printf("|%-11s ", aux->nombre);
            printf("| %-2i ", aux->existencia);
            printf("|%-16s", aux->previa);
            printf("|%-12s", aux->posterior);
            printf("|%-3i", aux->num_pokedex);
            printf("|%-7s|", aux->region);
            int tipos = aux->totalTIPOS;
            for(int i = 0; i < tipos; i++){
              printf("%s ", aux->tipo[i]);
            }
            printf("\n");
            printf("\n\n");
          }
          aux = nextMap(pokedex_nombre);
        }
        
        break;
 
    }
    x = next(datoUNICO);
  }
  if(check == false)
    printf("No existe ese pokemon\n\n");
}
  

//OPCION 7: Mostrar los pokemones registardos(POKEDEX) | LISTO
void mostrarPOKEDEX(Map * pokedex_NOMBRE)
{
  Pokedex * aux = firstMap(pokedex_NOMBRE);

  //Tamano del arreglo. Es una variable contadora con respecto el total de los Pokemons del almacenamiento
  int tamano = 0;

  while(aux != NULL)
  {
    tamano++;
    aux = nextMap(pokedex_NOMBRE);
  }

  int num[tamano];

  aux = firstMap(pokedex_NOMBRE);

  //Indices para los arreglos...
  int tipos, i, k;
  i = 0;

  //El arreglo sub "i" adquiere el valor del numero de la Pokedex del mapa
  while(aux!= NULL)
  {
    num[i] = aux->num_pokedex;
    i++;
    aux = nextMap(pokedex_NOMBRE);
  }

  //Este arreglo se ordena con el QuickSort
  qsort(num, tamano, sizeof(int), compararEnteros);

  aux = firstMap(pokedex_NOMBRE);
  if(aux == NULL)
  {
    printf("No hay pokemons registrados en la Pokedex...");
  }
  else
  {
    
    printf("\nNombre       |cant| Previo         |Posterior   |Num|Region | Tipo(s)\n");
    printf("--------------------------------------------------------------------------\n");
    for(k = 0; k < tamano; k++)
    {
      while(1)
      {
        //Se busca la coincidencia entre los numeros entre arreglo y mapa (Pokedex)
        if(num[k] == aux->num_pokedex)
        {
          printf("|%-11s ", aux->nombre);
          printf("| %-2i ", aux->existencia);
          printf("|%-16s", aux->previa);
          printf("|%-12s", aux->posterior);
          printf("|%-3i", aux->num_pokedex);
          printf("|%-7s|", aux->region);

          tipos = aux->totalTIPOS;
          for(i = 0; i < tipos; i++)
            printf("%s ", aux->tipo[i]);
      
          printf("\n");
          aux = nextMap(pokedex_NOMBRE);
          if(aux == NULL)
            aux = firstMap(pokedex_NOMBRE);

          break;
        }
        aux = nextMap(pokedex_NOMBRE);
        if(aux == NULL)
          aux = firstMap(pokedex_NOMBRE);
      }
    }
  }
  printf("\n\n");
}

//OPCION 8 | LISTO

void mostrarALMACENAMIENTO(Map * Almacen_ID){

  printf("\nINFORMACION:\n|ID | NOMBRE      | PC  | PS  |\n");
  pokemon * aux1 = firstMap(Almacen_ID);

  //Obtiene el tamano para el arreglo
  int tamano = 0;
  while(aux1 != NULL)
  {
    tamano++;
    aux1 = nextMap(Almacen_ID);
  }

  aux1 = firstMap(Almacen_ID);
  
  int pc[tamano];
  int i = 0;

  //Rellena cada casilla con los valores de los Puntos de Combate
  while(aux1 != NULL)
  {
    pc[i] = aux1->pc;
    i++;
    aux1 = nextMap(Almacen_ID);
  }
  
  //Se ordena por QuickSort 
  qsort(pc, tamano, sizeof(int), compararEnteros);
  
  aux1 = firstMap(Almacen_ID);
  
  for(i = 0; i < tamano; i++)
  {
    //Es posible que el "current" se reinicie desde el first para evitar datos repetidos
    while(1)
    {
      //Busca coincidencia entre arreglo y mapa (Alamcenamiento)
      if(pc[i] == aux1->pc)
      {
        printf("|%-3i| %-12s| %-4i| %-4i|\n", aux1->id, aux1->nombre, aux1->pc, aux1->ps);
        aux1 = nextMap(Almacen_ID);
        if(aux1 == NULL)
          aux1 = firstMap(Almacen_ID);
          
        break;
      }
      aux1 = nextMap(Almacen_ID);
      if(aux1 == NULL)
        aux1 = firstMap(Almacen_ID);
    }
  }

  printf("\n\n");
}

//Funcion para QuickSort, compara con 2 valores
int compararEnteros(const void* a, const void* b)
{
  int *ptrA = (int *)a;
  int *ptrB = (int *)b;

  //Pregunto si el primero es mayor que el segundo
  if( *ptrA > *ptrB)
    return 1; //Se intercambia
  else
    return 0; //Se mantienen
}


//OPCION 9 | LISTA

void liberarPOKEMON(Map * Almacen_ID, Map * pokedex_NOMBRE)
{
  
  int aux_id;
  char aux_nombre[20];

  printf("\nIngrese ID de Pokémon a eliminar: \n");
  scanf("%d", &aux_id);

  //Pregutamos si el almacenamiento esta vacio y si esta el ID
  if(searchMap(Almacen_ID, &aux_id) != NULL  && Almacen_ID != NULL)
  {
    pokemon * p = firstMap(Almacen_ID);
    while(p->id != aux_id)
      p = nextMap(Almacen_ID);

    //Se guarda el nombre para buscar en la Pokedex
    strcpy(aux_nombre, p->nombre);

    eraseMap(Almacen_ID, &aux_id);

    Pokedex * x = firstMap(pokedex_NOMBRE);

    //Busco para disminuir una unidad a su existencia
    while(strcmp(aux_nombre, x->nombre) != 0)
      x = nextMap(pokedex_NOMBRE);

    x->existencia--;

    printf("El Pokemon fue liberado exitosamente...\n\n");
  }
  else
    printf("El ID que ingreso no existe o habia sido eliminado del almacenamiento\n\n");
}

//OPCION 10: MOSTRAR POKEMON POR REGION | LISTA

void buscarPOKEMON_region(Map * pokedex_nombre, List * datoUNICO){
  char region[20];
  buscar * x = first(datoUNICO);
  getchar();
  printf("Ingrese la región del pokemon: ");
  scanf("%s", region);

  //Cuenta los Pokemons que hay en tal region
  int cont = 0;
  
  while(x != NULL)
  {
    //Busca coincidencias...
    if(strcmp(x->country , region) == 0)
    {
      //Imprime la informacion e incrimenta en 1 unidad a la variable contadora
      printf("\nNombre       |cant| Previo         |Posterior   |Num| Tipo(s)\n");
      printf("--------------------------------------------------------------------------\n");
      Pokedex * aux = firstMap(pokedex_nombre);
      while(aux != NULL)
      {
        if(strcmp(aux->region, region) == 0)
        {
          cont++;
          printf("|%-11s ", aux->nombre);
          printf("| %-2i ", aux->existencia);
          printf("|%-16s", aux->previa);
          printf("|%-12s", aux->posterior);
          printf("|%-3i|", aux->num_pokedex);

          int tipos = aux->totalTIPOS;
          for(int i = 0; i < tipos; i++){
            printf("%s", aux->tipo[i]);
          }
          printf("\n");
      

          
        }
        aux = nextMap(pokedex_nombre);
        
      }
      break;
      
    }
    x = next(datoUNICO);
  }
  //Se imprime los Pokemons que conto
  printf("\nUsted posee %i Pokémons provenientes de %s",cont, region);
  
  printf("\n\n");
}

//MENU DE OPCIONES...
void menu()
{ 
  printf(" ______________________________________\n");
  printf("|         Bienvenido al Menú de        |\n");
  printf("|          tu Pokedex! (v1.00)         |\n");
  printf("|______________________________________|\n");
  printf("|  1) Importar archivo pokemon         |\n");   
  printf("|  2) Atrapar un pokemon               |\n");   
  printf("|  3) Evolucion Pokemon                |\n"); 
  printf("|  4) Buscar pokemon por tipo          |\n");
  printf("|  5) Buscar mis pokemons por nombre   |\n");
  printf("|  6) Buscar pokemon por Pokedex       |\n");
  printf("|  7) Mostrar pokemons de la Pokedex   |\n");
  printf("|  8) Mostrar mis pokemons por PC      |\n");
  printf("|  9) Liberar Pokemon                  |\n");
  printf("| 10) Mostrar pokemons por Region      |\n");
  printf("|                                      |\n");
  printf("|  0) Salir                            |\n");
  printf("|______________________________________|\n");
  printf("¿Qué deseas hacer hoy?\n");
}
