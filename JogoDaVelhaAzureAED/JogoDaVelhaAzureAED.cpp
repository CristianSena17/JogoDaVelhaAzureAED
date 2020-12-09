//Trabalho Prático Final AED - Jogo da Velha com Reconhecimento de Voz
//Cristian Fernandes Sena - Engenharia de Computação Coreu

#include <iostream>
#include <fstream>
#include <string>
#include <speechapi_cxx.h> //Biblioteca do serviço de fala    
#include <locale.h>

using namespace std;

//Serviços cognitivos da Microsoft 
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;

auto autenticacao = SpeechConfig::FromSubscription("e152447a06704466b06306bbcbbfc7bc", "southcentralus"); //Declaração da autenticação do recurso
auto requisicao_textofala = SpeechSynthesizer::FromConfig(autenticacao);                                  //Declaração do objeto de requisição de texto em fala do recurso
auto audio_config = AudioConfig::FromDefaultMicrophoneInput();                                            //Declaração da entrada do microfone
auto requisicao_falatexto = SpeechRecognizer::FromConfig(autenticacao, audio_config);                     //Declaração do objeto de requisição de fala em texto do recurso


/*
Descrição: Função que verifica se existe alguma combinação vencedora e retorna um valor bool.
Entradas: A matriz de char do jogo.
Saída: Um valor bool verdadeiro que indica se há uma vitória ou não.
*/
bool Verificacao(char matriz[3][3]) {
    if (((matriz[0][0] == matriz[0][1]) && (matriz[0][0] == matriz[0][2]) && (matriz[0][0] != '_')) ||
        ((matriz[0][0] == matriz[1][0]) && (matriz[0][0] == matriz[2][0]) && (matriz[0][0] != '_')) ||
        ((matriz[0][1] == matriz[1][1]) && (matriz[0][1] == matriz[2][1]) && (matriz[0][1] != '_')) ||
        ((matriz[0][2] == matriz[1][2]) && (matriz[0][2] == matriz[2][2]) && (matriz[0][2] != '_')) ||
        ((matriz[1][0] == matriz[1][1]) && (matriz[1][0] == matriz[1][2]) && (matriz[1][0] != '_')) ||
        ((matriz[2][0] == matriz[2][1]) && (matriz[2][0] == matriz[2][2]) && (matriz[2][0] != '_')) ||
        ((matriz[0][0] == matriz[1][1]) && (matriz[0][0] == matriz[2][2]) && (matriz[0][0] != '_')) ||
        ((matriz[2][0] == matriz[1][1]) && (matriz[2][0] == matriz[0][2]) && (matriz[2][0] != '_'))) {
        return true; //Existe um vencedor   
    }
    return false; //Ainda não existe um vencedor   
}


/*
Descrição: Procedimento que requisita da API a transformação de um texto em fala.
Entradas: Um texto em uma string.
Saída: Sintetização de texto em fala.
*/
void texto_em_fala(string Texto){
    setlocale(LC_ALL, "portuguese");
    cout << Texto + "\n"; //Imprime o texto recebido
    requisicao_textofala->SpeakTextAsync(Texto).get(); //Requisição da sintetização de texto em fala
}


/*
Descrição: Função que requisita da API o reconhecimento de uma fala e a transformação dessa fala em um texto.
Entradas: Não Possui.
Saída: O texto reconhecido em string.
*/
string fala_em_texto() {
    setlocale(LC_ALL, "portuguese");
    auto resultado = requisicao_falatexto->RecognizeOnceAsync().get(); //Requisição do reconhecimento de fala em texto
    cout << resultado->Text + "\n"; //Imprime o texto reconhecido
    return resultado->Text; //Conversão do resultado do reconhecimento em texto
}

void imprimeMatriz(char matriz[3][3]) {
    //Imprime a matriz
    for (int i = 0; i < 3; i++) {
        if (i > 0)
            cout << "\n";
        for (int j = 0; j < 3; j++) {
            if (j > 0)
                cout << "\t";
            cout << matriz[i][j];
        }
        cout << "\n";
    }
}

/*
Descrição: Função principal.
Entradas: Valores ditos pelo usuário em onde jogar.
Saída: um valor inteiro 0 e as chamadas de funções.
*/
int main() {
    cout << "AZURE - SERVICO COGNITIVO DE FALA - JOGO DA VELHA\n";
    string key = "", region = "";
    cout << "Informe a key do recurso: ";
    cin >> key;                                                                      //Entrada da chave do recurso
    cout << "Informe a regiao em que o recurso e hospedado: ";
    cin >> region;                                                                   //Entrada da região em que o recurso foi hospedado
    cout << "--------------------------------------------------------------\n";

    autenticacao = SpeechConfig::FromSubscription(key, region);                      //Autenticação do recurso com a chave e região informados pelo usuário
    autenticacao->SetSpeechRecognitionLanguage("pt-BR");                             //Configuração da autenticação para o reconhecimento de fala em português 
    autenticacao->SetSpeechSynthesisLanguage("pt-BR");                               //Configuração da autenticação para a sintetização de fala em português 
    autenticacao->SetSpeechSynthesisVoiceName("pt-BR-Daniel"); //pt-BR-Daniel        //Configuração da autenticação com uma voz específica
    requisicao_textofala = SpeechSynthesizer::FromConfig(autenticacao);              //Redefinição do objeto requisicao_textofala com as novas configurações
    requisicao_falatexto = SpeechRecognizer::FromConfig(autenticacao, audio_config); //Redefinição do objeto requisicao_falatexto com as novas configurações

    setlocale(LC_ALL, "portuguese"); //Para a utilização de acentuações 

    texto_em_fala("Jogo iniciado");

    int i, j;
    int m = -1, n = -1; //Variáveis das posições de jogadas (iniciadas com valores inválidos)
    int jogador; //Determina a vez de cada jogador

    //Variáveis que armazenam os valores convertidos de voz em texto
    string linha; 
    string coluna;

    //Matriz Tabuleiro 
    char matriz[3][3];

    //Controla o número máximo de jogadas  
    for (jogador = 0; jogador < 9; jogador++) {

        //Prepara a matriz na primeira jogada
        if (jogador == 0) {
            for (i = 0; i < 3; i++) { 
                for (j = 0; j < 3; j++) { 
                    matriz[i][j] = '_'; 
                }
            }
            //Imprime a matriz
            imprimeMatriz(matriz);
        }

        do {
            try {
                //Pergunta e ouve a linha e coluna de jogada do usuário 
                cout << "\n";
                if(jogador%2==0) //Verifica qual o jogador
                    texto_em_fala("Primeiro jogador, em qual linha você deseja jogar?");
                else
                    texto_em_fala("Segundo jogador, em qual linha você deseja jogar?");

                linha = fala_em_texto();

                if (jogador % 2 == 0)//Verifica qual o jogador
                    texto_em_fala("Primeiro jogador, em qual coluna você deseja jogar?");
                else
                    texto_em_fala("Segundo jogador, em qual coluna você deseja jogar?");

                coluna = fala_em_texto();
            }
            catch (exception e)
            {
                cout << e.what();
            }

            //Converte o texto ouvido em um inteiro em m
            if (linha == "Um." || linha == "Um" || linha == "1." || coluna == "um." || coluna == "um" || coluna == "1") {
                m = 0;
            }
            else if (linha == "Dois." || linha == "Dois" || linha == "2." || linha == "Pois.") {
                m = 1;
            }
            else if (linha == "Três." || linha == "Três" || linha == "3.") {
                m = 2;
            }
            
            //Converte o texto ouvido em um inteiro em n 
            if (coluna == "Um." || coluna == "Um" || coluna == "1." || coluna == "um." || coluna == "um" || coluna == "1") {
                n = 0;
            }
            else if (coluna == "Dois." || coluna == "Dois" || coluna == "2." || linha == "Pois.") {
                n = 1;
            }
            else if (coluna == "Três." || coluna == "Três" || coluna == "3.") {
                n = 2;
            }

            //Verifica se os valores são validos  
            if (((m < 0 || m > 2) || (n < 0 || n > 2))) {
                //Limpa a tela
                system("cls");
                //Imprime a matriz
                imprimeMatriz(matriz);
                cout << "\n";
                texto_em_fala("Valores Inválidos, tente noamente");
            }
        } while ((m < 0 || m > 2) || (n < 0 || n > 2)); //Repete até que sejam inseridos valores validos   

        cout << "\n";

        //Verifica se a posição está vazia
        if (matriz[m][n] == '_') {
            //Coloca o símbolo de cada jogador
            if (jogador % 2)
                matriz[m][n] = 'O';
            else
                matriz[m][n] = 'X';

            //Limpa a tela
            system("cls");

            //Imprime a matriz
            imprimeMatriz(matriz);

            //Verifica o número mínimo de jogadas para que exista um vencedor 
            if (jogador >= 4) {
                if (Verificacao(matriz)) { //Verifica se há um vencedor  
                    if ((jogador % 2) == 0) { //Verifica qual jogador ganhou
                        cout << "\n";
                        texto_em_fala("O primeiro jogador ganhou");
                    }
                    else {
                        cout << "\n";
                        texto_em_fala("O segundo jogador ganhou");
                    }
                    exit(0);
                }
            }
        }
        else { //Informa que o local escolhido já esta preenchido  
            //Limpa a tela
            system("cls");
            //Imprime a matriz
            imprimeMatriz(matriz);
            cout << "\n";
            texto_em_fala("Uma jogada já foi feita neste local, jogue novamente");
            jogador--;
        }
    }

    cout << "\n";
    texto_em_fala("Deu velha. Jogo encerrado"); //Informa que não houve vencedores

    return 0;
}