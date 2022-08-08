/**
 **===========================================================================================================================
 ** Elementos de Programa��o
 ** Professor: Marcos Zuccolotto
 ** Alunos: Lu�s Gustavo Lauermann Hartmann e Pedro Stanislau Tomacheski
 ** Turma: 4311
 ** Obs: Neste trabalho recebemos assist�ncia de Pedro Lawisch e discutimos as dificuldades com o grupo do Samuel e Jo�o Vitor
 **===========================================================================================================================
 **
 **===========================================================================================================================
 ** Trabalho Pr�tico - Pegando Onda
 ** Objetivo:
 **   Exercitar os conte�dos listados abaixo atrav�s de uma aplica��o de
 **   manipula��o de arquivos de audio.
 **  - Acesso a arquivos
 **  - Arrays & ponteiros
 **  - Structs
 **
 **============================================================================================================================
 */


// Includes:
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <locale.h>
#include <conio.h>
#include <windowsx.h>
#include <stdint.h>



/*
    Essa biblioteca, 'stdint.h', serve para termos uma defini��o correta do tamanho de cada tipo de vari�vel. Int pode
    valer tanto 2, quanto 4. Esse valor depende do compilador. Essa biblioteca serve, portanto, para obtermos
    uma defini��o correta e fixa quanto ao valor de um int em bytes.
 */

// === ESTRUTURA CONTENDO TODAS AS PARTES DO ARQUIVO WAVE === //
typedef struct
{
	// - RIFF Header
	char ChunkID[4];
	uint32_t ChunkSize;
	char Format[4];

	// - 'fmt ' Subchunk
	char Subchunk1ID[4];
	uint32_t Subchunk1Size;
	uint16_t AudioFormat;
	uint16_t  NumChannels;
	uint32_t SampleRate;
	uint32_t ByteRate;
	uint16_t  BlockAlign;
	uint16_t  BitsPerSample;

	// -'data' Subchunk
	char Subchunk2ID[4];
	uint32_t  Subchunk2Size;
} WAV;

// === Vari�veis globais === //
char fname[100],nomesemwav[100]; // === Vari�vel do nome do arquivo e seu nome sem o .wav concatenado.

FILE *arq_origem,*arq_mod; // === Ponteiros para o arquivo original e o que ser� moridicado

WAV arquivo, copia;

int tam_mdados = 0; // Vari�vel que armazena o tamanho dos metadados, e estes ser�o contados na fun��o ColetaDados

short int mdados_meio = -1; // Vari�vel que indicar� se os metadados est�o no meio do arquivo ou no final. Se valer 1, est�o no
// meio, se valer 0, os metadados estar�o juntos ao 'data'.


// ::::::::::::::::: START FUN��ES :::::::::::::::::::::::

    // === FUN��O PARA EXIBIR O CABE�ALHO === //
        void cabecalho(void)
        {
            system("cls");
            puts(" TRABALHO DE PROGRAMA��O - PEGANDO ONDA");
            puts(" Lu�s Gustavo Lauermann Hartmann");
            puts(" Pedro Stanislau Tomacheski");
            puts(" TURMA 4311");
            puts(" Este programa serve para ler e manipular arquivos '.WAV'");
        }
    // === FUN��O PARA EXIBIR O CABE�ALHO === //


    // === FUN��O PARA EXIBIR O MENU === //

        int menu(void)
        {
            char escolha;
            do
            {
                system("cls");
                printf(" ============================================\n");
                printf(" As seguintes opera��es s�o poss�veis:\n");
                printf(" 1.Informar sobre os detalhes do arquivo .WAV.\n");
                printf(" 2.Modificar o volume do arquivo .WAV.\n");
                printf(" 3.Recortar um trecho do arquivo .WAV.\n");
                printf(" 4.Adicionar efeitos no arquivo .WAV.\n");
                printf(" 5.Ouvir o conte�do do arquivo .WAV.\n");
                printf(" 6.Parar de reproduzir o conte�do do arquivo .WAV.\n");
                printf(" 7.Abrir um novo arquivo .WAV.\n");
                printf(" ===========================================\n");
                printf(" Obs: digite ESC para sair do programa.\n");
                printf("\n Sua decis�o: ");
                escolha = getch();
            }
            while((escolha<'1' || escolha>'7')&& escolha!=27);
            return escolha;
        }
    // === FUN��O PARA EXIBIR O MENU === //


    // === FUN��O PARA EXIBIR O MENU DE EFEITOS=== //
        int menu_efeitos(void)
        {
            char escolha;
            do
            {
                system("cls");
                printf(" ============================================\n");
                printf(" Os Seguintes efeitos s�o poss�veis:\n");
                printf(" 1.Distor��o.\n");
                printf(" 2.Vibrato.\n");
                printf(" 3.Echo\n");
                printf(" ===========================================\n");
                printf("\n Sua decis�o: ");
                escolha = getch();
            }
            while(escolha<'1' || escolha>'3');
            return escolha;
        }
    // === FUN��O PARA EXIBIR O MENU DE EFEITOS=== //

    // === FUN��O QUE L� O AQUIVO DESEJADO === //
        int ColetaDados ()
        {
            arq_origem = fopen (fname,"rb");

            // === PRINTANDO O ERRO, CASO O ARQUIVO .WAV N�O EXISTIR === //
                if(arq_origem == NULL)
                {
                    system("cls");
                    printf(" O arquivo .wav selecionado n�o existe!\n");
                    system("pause");
                    return -1; // Esse return 1 serve para que a fun��o main possa pedir novamente o nome do arquivo.
                }
            // === PRINTANDO O ERRO, CASO O ARQUIVO .WAV N�O EXISTIR === //


            // === LENDO O HEADER INTEIRO === //
                fread(&arquivo,sizeof(arquivo)-8,1,arq_origem); // Passa todos os dados para a struct at� o BitsPerSample.
                /*
                    Na struct WAV, cada vari�vel tem seu tipo e tamanho definido. Sendo assim,
                    sempre que a leitura 'esgotar' o tamanho de bits de uma vari�vel, ela usa
                    outra vari�vel para armazenar os pr�ximos dados a serem lidos, usando a pr�xima
                    vari�vel da struct.
                */

                /*
                    A partir daqui, ser� feito um teste para sabermos se os metadados est�o no 'data'
                    ou entre o  BitsPerSample e o Subchunk2ID
                */
            // === LENDO O HEADER INTEIRO === //


            char checa_data[5];
            checa_data[5] = '\0';

            /*
                Como n�o se sabe o tamanho dos metadados, precisaremos procur�-los de alguma forma.
                Esta forma ser� buscar a str "data", pois da� saberemos que o ponteiro de arq_origem estar�
                em Subchunk2ID.
            */

            do
            {
                for(int i=0;i<4;i++)
                {
                    checa_data[i] = fgetc(arq_origem);
                }

                fseek(arq_origem,-7,SEEK_CUR);
                tam_mdados++;

            }while (strcmp(checa_data,"data"));

            tam_mdados-=4;

            if(tam_mdados>0)
            {
                mdados_meio = 1;
            }

            else
            {
               mdados_meio = 0;
            }

            fseek(arq_origem,3,SEEK_CUR);

            fread(arquivo.Subchunk2ID,1,4,arq_origem);// Aloca a str "data" no Subchunk2ID.

            fread(&arquivo.Subchunk2Size,1,4, arq_origem);
            /* Como fizemos um while para posicionar o ponteiro no Subchunk2ID, basta
               passarmos o resto da informa��o para a estrutura.
            */


            fclose(arq_origem);

            // === DEFININDO O TAMANHO DOS METADADOS === //
                tam_mdados = arquivo.ChunkSize - 36 - arquivo.Subchunk2Size;
            // === DEFININDO O TAMANHO DOS METADADOS === //
        }

    // === FUN��O QUE L� O AQUIVO DESEJADO === //


    // === PRINTA O ID SEM LETRAS ADICIONAIS === //
        void printID(char *posicao)
        {
            // Para evitar letras indesejadas, printaremos apenas 4 letras de cada identificador.
            for(int i=0; i<4; i++)
            {
                printf("%c",*(posicao+i)); // Printa uma letra do ID.
            }
            printf("\n");
        }
    // === PRINTA O ID SEM LETRAS ADICIONAIS === //


    // === FUN��O QUE EXIBE O CABE�ALHO DO ARQUIVO .WAVE === //
        void MostraInfos()
        {
            system ("cls");
            puts("=== RIFF Chunk ===");
            printf("ChunkID: ");
            printID(arquivo.ChunkID);

            printf("ChunkSize: %i bytes\n",arquivo.ChunkSize);

            printf("Format: ");
            printID(arquivo.Format);

            puts("\n=== 'fmt ' SubChunk ===");
            printf("SubChunk1ID: ");
            printID(arquivo.Subchunk1ID);

            printf("SubChunk1Size: %i bytes\n",arquivo.Subchunk1Size);
            printf("AudioFormat: %i",arquivo.AudioFormat);

            switch(arquivo.AudioFormat)
            {
                case 1:
                    printf(" (PCM)\n");
                break;

                case 2:
                    printf(" (Comprimido)\n");
                break;
                default:
                    printf(" (Others)\n");
                break;
            }

            printf("NumChannels: %i canais ->",arquivo.NumChannels);

            switch(arquivo.NumChannels)
            {
                case 1:
                    printf(" (Mono)\n");
                break;

                case 2:
                    printf(" (Stereo)\n");
                break;

                default:
                    printf(" (Others)\n");
                break;
            }

            printf("SampleRate: %i amostras por segundo \n",arquivo.SampleRate);
            printf("ByteRate: %i bytes\n",arquivo.ByteRate);
            printf("BlockAlign: %i\n",arquivo.BlockAlign);
            printf("BitsPerSample: %i bits por amostra\n",arquivo.BitsPerSample);

            puts("\n=== 'data' SubChunk ===");
            printf("SubChunk2ID: ");
            printID(arquivo.Subchunk2ID);
            printf("SubChunk2Size: %i bytes\n",arquivo.Subchunk2Size);
        }
    // === FUN��O QUE EXIBE O CABE�ALHO DO ARQUIVO .WAVE === //


    // === FUN��O PARA CORTAR A M�SICA === //
        void CortaMusica ()
        {
            int inicio=0,fim=0;
            char nome_cut[100];

            // === DEFININDO O NOME DO ARQUIVO QUE SER� ABERTO === //
            strcpy(nome_cut,nomesemwav);
            strcat(nome_cut,"_corte_mod.wav");

            // === PEDINDO O SEGUNDO INICIAL E O SEGUNDO FINAL === //
            short int erro =0;
            do{
                if(erro==1)
                    printf("\n Um dos par�metros � inv�lido, tente novamente.\n");
                puts("\n Digite o segundo inicial: ");
                scanf("%d",&inicio);
                puts(" Digite o segundo final : ");
                scanf("%d",&fim);
                erro = 1;
            }while (inicio>=fim || inicio<0 || (fim> (int)arquivo.Subchunk2Size/arquivo.ByteRate));
            // Se o fim for maior que o tamanho total da m�sica em segundos, repete.



            // === DEFININDO O TAMANHO DO CORTE EM BYTES === //
            int tam_corte = ( fim - inicio )*(arquivo.ByteRate); // Tamanho do Subchunk2Size do arquivo que ser� cortado em bytes



            // === REDEFININDO O TAMANHO DO CHUNKSIZE E SUBCHUNK2SIZE === //
            copia.ChunkSize = 36 + tam_corte + tam_mdados;
            copia.Subchunk2Size = tam_corte;



            // === ABRINDO O ARQUIVO ORIGINAL E UM NOVO ARQUIVO === //
            arq_origem = fopen(fname,"rb");
            arq_mod = fopen(nome_cut,"wb");



            // === ESCREVENDO O CABE�ALHO NO SEGUNDO ARQUIVO === //
            fwrite(&copia,sizeof(arquivo),1,arq_mod);
            printf(" Transfer�ncia do cabe�alho.....:ok\n");

            // === CHEACANDO SE OS METADADOS EST�O NO FINAL OU NO COME�O === //
            if(!mdados_meio)// Se os metadados n�o estiverem no meio, entre o Subchunk2ID e o BitsPerSample...
            {
                // Aponta o ponteiro para o 'data' do arquivo origem.
                fseek(arq_origem,44,SEEK_SET);
            }
            else
            {
                // Aponta o ponteiro para o 'data' do arquivo origem considerando o tam_mdados, ou seja, ele pula tudo e os mdados tamb�m.
                fseek(arq_origem,tam_mdados+44,SEEK_SET);
            }

            /*
                Considerando que o ponteiro j� est� no 'data', basta-nos apont�-lo para o momento certo em que a m�sica
                dever� ser inicialmente cortada.
            */

            // === DIRECIONANDO O PONTEIRO PARA O SEGUNDO INICIAL EM BYTES === //
            fseek(arq_origem,inicio*(arquivo.ByteRate), SEEK_CUR);



            // === COPIANDO OS DADOS AT� O FINAL DEFINIDO === //
            for(int c=0;c<tam_corte;c++)
            {
                fputc(fgetc(arq_origem),arq_mod);
            }
            printf(" Transfer�ncia do 'data'........:ok");


            //O data do arquivo j� foi copiado, agora temos de passar os metadados para o arquivo modificado.

            // === APONTANDO O PONTEIRO PARA O LOCAL ONDE EST�O OS METADADOS === //
            if(mdados_meio) // Se os metadados estiverem no meio ...
                fseek(arq_origem,36,SEEK_SET); // Posiciona o ponteiro entre o BitsPerSample e o Subchunk2ID.
            else
                fseek(arq_origem,(arquivo.ChunkSize + 8 - tam_mdados),SEEK_SET); // Posiciona o ponteiro exatamente no inicio dos mdados.



            // === TRANSFERINDO OS METADADOS PARA O ARQ_MOD === //
            for(int c=0;c<tam_mdados;c++)
            {
                fprintf(arq_mod,"%c",fgetc(arq_origem));
            }

            printf("\n Transfer�ncia dos metadados....:ok\n");
            printf("\n Arquivo de armazenamento: %s\n",nome_cut);

            fclose(arq_mod);
            fclose(arq_origem);
        }
    // === FUN��O PARA CORTAR A M�SICA === //


    // === FUN��O PARA MANINUPULAR O VOLUME DA M�SICA === //
    void ModificaVolume()
    {
        float coeficiente; // Coeficiente pelo qual os valores da m�sica ser�o multiplicados.
        char nome_vol[100]; // String que armazena o nome do arquivo que ter� seu volume modificado.
        int16_t amostra; // Vari�vel que armazena uma amostra, ser� usada diversas vezes.
        char data[5]; // Esta vari�vel armazenar� o que h� no Subchunk2ID.
        int i=0, cont; // Vari�veis auxiliares

        strcpy(nome_vol,nomesemwav);
        strcat(nome_vol,"_vol_mod.wav");


        do
        {
            printf("\n Coeficiente de multiplica��o do volume (menor que 1 diminui o som): ");
            scanf("%f",&coeficiente);
        }
        while (coeficiente < 0);


        // Abrindo o arquivo original para poder l�-lo e repassar o data para o outro arquivo
        // e modificar o volume.
        arq_origem = fopen (fname, "rb");

        // Abrindo o arquivo no qual ser� armazenado o data multiplicado por 'coeficiente'.
        arq_mod = fopen (nome_vol,"wb");



        // Escrevendo o cabe�alho no novo arquivo.
        fwrite(&arquivo,sizeof(WAV),1,arq_mod);
        /*
            Aqui vamos checar se os metadados est�o abaixo do Subchunk2Size ou entre o 'BitsPerSample' e
            o Subchunk2ID.
         */
        printf(" Transfer�ncia do cabe�alho.............:ok\n");

        if(!mdados_meio)
        {
            /*
                Caso o Subchunk2ID realmente esteja como o planejado, os metadados estar�o juntos com o 'data'.
                Neste caso, pode-se realizar simplesmente 1 varredura transferindo o 'data'*coeficiente para
                o arq_vol. Para isso, precisaremos apenas transferir o ponteiro arq_vol do in�cio do arquivo
                arq_origem para a parte do 'data', que fica 44 bytes � frente.
             */
            fseek(arq_origem,44,SEEK_SET); // A op��o SEEK_SET serve para mostrarmos � fun��o que esses 44 bytes
            // de 'offset' devem ser adicionados � posi��o inicial do arquivo.
        }
        else
        {
            /*
               Caso o ID "data" n�o esteja no Subchunk2ID, significa que os metadados est�o entre o BitsPerSample e
               o Subchunk2ID. Neste caso, precisa-se copiar os metadados e o data em 2 varreduras diferentes. Setaremos
               o ponteiro, primeiramente, no data, sendo assim, os metadados ficar�o por �ltimo no arquivo, o que � uma
               estrutura comum do .wav.
             */
            fseek(arq_origem,tam_mdados+44,SEEK_SET);
            /*
                SEEK_CUR � a fun��o que posiciona o ponteiro onde ele realmente est�, e n�o no in�cio do arquivo.
                tam_data+8 � a adi��o � posi��o inicial necess�ria para chegarmos ao 'data' em si. tam_mdata para pularmos
                os metadados e 8 para pularmos o Subchunk2ID e o Subchunk2Size.
             */
        }

        // Come�ando a copiar o 'data'.
        while ( i < arquivo.Subchunk2Size)
        {
            i+=arquivo.BlockAlign; // Pega amostras de acordo com o n�mero de bytes de cada amostra;
            amostra = fgetc(arq_origem);

            amostra |= (fgetc(arq_origem)<<8);

            if((int)(abs(amostra)*coeficiente) >= 32767.)
            {
                if(amostra > 0)
                    amostra = 32767;
                if(amostra < 0)
                    amostra = -32768;
            }

            else
            amostra = (int16_t) amostra*coeficiente;

            fwrite(&amostra,sizeof(short),1,arq_mod);
        }
        printf(" Transfer�ncia do 'data' manipulado.....:ok\n");

        // === TRANSFERINDO OS METADADOS === //
        if(mdados_meio)
            fseek(arq_origem,36,SEEK_SET);
        i=0;
        while (i++<tam_mdados)
        {
            fprintf(arq_mod, "%c",fgetc(arq_origem));
        }
        printf(" Transfer�ncia dos metadados............:ok\n");
        printf("\n Arquivo de armazenamento: %s \n",nome_vol);

        fclose(arq_origem);
        fclose(arq_mod);
    }

    void AdicionaEco (void)
    {
        int16_t  aux,amostra1,amostra2=0;
        char nome_eco[100]; // String que armazena o nome do arquivo que ter� o efeito adicionado.
        int i=0;

        strcpy(nome_eco,nomesemwav);
        strcat(nome_eco,"_eco_mod.wav");

        // Abrindo o arquivo original para poder l�-lo e repassar o data para o outro arquivo
        // e adicionar eco.
        arq_origem = fopen (fname, "rb");

        // Abrindo o arquivo no qual ser� armazenado o data multiplicado por 'coeficiente'.
        arq_mod = fopen (nome_eco,"wb");


        // Escrevendo o cabe�alho no novo arquivo.
        fwrite(&arquivo,sizeof(WAV),1,arq_mod);
        /*
            Aqui vamos checar se os metadados est�o abaixo do Subchunk2Size ou entre o 'BitsPerSample' e
            o Subchunk2ID.
         */
        printf(" Transfer�ncia do cabe�alho.............:ok\n");

        if(!mdados_meio)
        {
            fseek(arq_origem,44,SEEK_SET);
        }

        else
        {
            fseek(arq_origem,tam_mdados+44,SEEK_SET);
        }

        while ( i < arquivo.Subchunk2Size)
        {
            i+=arquivo.BlockAlign; // Pega amostras de acordo com o n�mero de bytes de cada amostra;
            amostra1 = fgetc(arq_origem);

            amostra1 |= (fgetc(arq_origem)<<8);

            aux = amostra1;

            if((int)(amostra1 + amostra2) >= 32767)
            {
                if((amostra1+amostra2) > 0)
                    amostra1 = 32767;

                if((amostra1+amostra2) < 0)
                    amostra1 = -32768;
            }

            else
            amostra1 = (int16_t) (amostra1 + amostra2);

            fwrite(&amostra1,sizeof(short),1,arq_mod);

            amostra2  = aux;
        }

        printf("\n Transfer�ncia do 'data' manipulado.....:ok\n");

        // === TRANSFERINDO OS METADADOS === //
        if(mdados_meio)
            fseek(arq_origem,36,SEEK_SET);
        i=0;

        while (i++<tam_mdados)
        {
            fprintf(arq_mod, "%c",fgetc(arq_origem));
        }

        printf(" Transfer�ncia dos metadados............:ok\n");
        printf("\n Arquivo de armazenamento: %s \n",nome_eco);

        fclose(arq_origem);
        fclose(arq_mod);

    }


    // === FUN��O PARA PEDIR O DIRET�RIO === //
    void PedeDiretorio()
    {
        printf("\n\n");
        do
        {
            // === PEDINDO O DIRET�RIO DO ARQUIVO === //
            puts(" Digite o nome do arquivo WAVE desejado: ");
            gets(fname);
            strcpy(nomesemwav,fname);
            strcat(fname,".wav"); // Adiciona o texto '.wave' ao final da string 'fname'
        }
        while(ColetaDados(fname)==-1); // === COME�ANDO A COLETAR INFORMA��ES DO ARQUIVO === //
        printf("\n\n");
    }

    // === FUN��O PARA COPIAR O CABE�ALHO === //
    void CopiaInfos()
    {
        memcpy(&copia,&arquivo,sizeof(WAV));
    }

// ::::::::::::::::: END FUN��ES :::::::::::::::::::::::


// :::::::::::::::::: START MAIN :::::::::::::::::::::::
    int main ()
    {

        setlocale(LC_ALL,"portuguese");		// Habilita caracteres da l�ngua portuguesa
        // === DECLARA��O DE VARI�VEIS === //
        int escolha;

        // === PRINTANDO O CABE�ALHO === //
        cabecalho();

        PedeDiretorio();

        system ("cls");
        while (1)
        {
            // === CHAMANDO A FUN��O MENU E RECEBENDO A ESCOLHA === //
            escolha = menu();

            switch(escolha)
            {
                case 27:
                    puts("\n\n~~~~ Programa encerrado! ~~~~\n\n");
                    exit(1);
                break;

                case '1': // .Informar sobre os detalhes do arquivo .WAV.
                    MostraInfos(&arquivo);
                    printf("\n ");
                    system("pause");
                break;

                case '2':// Modificar o volume do arquivo .WAV.
                    ModificaVolume();
                    printf("\n ");
                    system("pause");
                break;

                case '3':// .Modificar o volume do arquivo .WAV.
                    CopiaInfos(); // renova os dados na copia
                    CortaMusica();
                    printf("\n ");
                    system("pause");
                break;

                case '4':
                    CopiaInfos(); // renova os dados na copia

                    escolha = menu_efeitos();

                    switch (escolha)
                    {
                        case '1':
                            AdicionaEco();
                        break;

                        case '2':


                        break;

                        case '3':


                        break;
                    }

                    system("pause");
                break;

                case '5': // Ouvir o conte�do do arquivo .WAV.
                    PlaySound(fname, NULL, SND_ASYNC);
                    printf("\n ");
                    system("pause");
                break;

                case '6': // Parar de reproduzir o conte�do do arquivo .WAV.
                    PlaySound(0,0,0);
                break;

                case '7': // Abrir um novo arquivo .WAV.
                    PedeDiretorio();
                break;
            }
        }
    }
// :::::::::::::::::: FINAL MAIN :::::::::::::::::::::::
