

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include "ProcessHacker.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ProcessHacker procHacker(L"ac_client.exe", 
		PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_CREATE_THREAD | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, ARCH_X86);

	procHacker.ProcLoadLibraryA("C:\\Users\\giova\\source\\repos\\InternalHack\\Debug\\HackDLL.dll");

	std::cout << "Injetado.";
}

// Executar programa: Ctrl + F5 ou Menu Depurar > Iniciar Sem Depuração
// Depurar programa: F5 ou menu Depurar > Iniciar Depuração

// Dicas para Começar: 
//   1. Use a janela do Gerenciador de Soluções para adicionar/gerenciar arquivos
//   2. Use a janela do Team Explorer para conectar-se ao controle do código-fonte
//   3. Use a janela de Saída para ver mensagens de saída do build e outras mensagens
//   4. Use a janela Lista de Erros para exibir erros
//   5. Ir Para o Projeto > Adicionar Novo Item para criar novos arquivos de código, ou Projeto > Adicionar Item Existente para adicionar arquivos de código existentes ao projeto
//   6. No futuro, para abrir este projeto novamente, vá para Arquivo > Abrir > Projeto e selecione o arquivo. sln
