// RBT.cpp: Красно-чёрные деревья
//=== Демонстрационная программа ================== (C)Сlgn: 25.04.07;09.03.14
//Построение дерева из упорядоченного массива и выполнение 20 вставок/удалений.
//Красные узлы помечаются "*"
//#include "pch.h"

#include <clocale>
#include <stdlib.h>
#include <string>
#include <stdarg.h>
#include <time.h>
#include <iostream>
#include <exception>
#include <vector>
#include <algorithm>
#include "RBTree.h"
using namespace std;



int main()
{
	srand((unsigned int)time(nullptr));
	std::setlocale(LC_ALL, "Russian");
	Tree t__; t__.Display(); system("cls"); t__.~Tree();


	Tree A(10, 15);	Tree B(9, 14); Tree C(10, 13); Tree D(7, 12); Tree E(6, 11);
	
	cout << "\n" << "---INPUT DATA---" << "\n";
	A.printSeq("A = "); B.printSeq("B = "); C.printSeq("C = "); D.printSeq("D = "); E.printSeq("E = ");


	cout << "\n" << "---SEQUENCES---" << "\n";
	A.merge(B); A.printSeq("A.MERGE(B) = ");
	B.concat(C); B.printSeq("B.CONCAT(C) = ");

	Tree half_C(C.getRandomSeq());
	C.excl(half_C); 
	cout << "C.EXCL("; for (auto x : half_C.seq) cout << " " << x; C.printSeq(" ) = ");

	cout << "\n" << "press any button...";
	cin.get(); system("cls");

	cout << "\n" << "---CHANGED DATA---" << "\n";
	A.printSeq("A = "); B.printSeq("B = "); C.printSeq("C = "); D.printSeq("D = "); E.printSeq("E = ");
	Tree *t1;
	cout << "\n";
	t1 = new Tree(A | B); t1->Display("Tree A | B "); t1->printSeq("Seq of this tree = "); 
	clrscr();  cout << "\n";
	t1 = new Tree(C & D); t1->Display("Tree C & D "); t1->printSeq("Seq of this tree = ");
	clrscr();  cout << "\n";
	t1 = new Tree(D - E); t1->Display("Tree D - E "); t1->printSeq("Seq of this tree = ");
	clrscr();  cout << "\n";
	t1 = new Tree(E ^ A); t1->Display("Tree E ^ A "); t1->printSeq("Seq of this tree = ");
	clrscr();  cout << "\n";

	cout << "\n" << "press any button...";
	cin.get(); system("cls");

	cout << "\n" << "---DATA---" << "\n";
	A.printSeq("A = "); B.printSeq("B = "); C.printSeq("C = "); D.printSeq("D = "); E.printSeq("E = ");

	t1 = new Tree(((A | B) & (C | D)) ^ E);
	t1->Display("((A | B) & (C | D)) ^ E)");
	t1->printSeq("Seq of this tree = ");
	

	return 0;
}