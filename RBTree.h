#pragma once

#include <clocale>
#include <stdlib.h>
#include <string>
#include <stdarg.h>
#include <time.h>
#include <iostream>
#include <exception>
#include <algorithm>

#include <iterator>
#include <stack>
#include <string>
#include <vector>

using namespace std;

enum Direction { left, right };
enum nodeColor { BLACK, RED };

// NODE 
// Узел красно-чёрного дерева
class Node {
	static Node* root;      // статический указатель на корень
	nodeColor color;	// цвет узла (BLACK, RED)
	void Destroy();		// Удаление дерева
	void rotate(Direction);	// Вращение
	void insertFixup();		// Восстановление после вставки
	void deleteFixup();		// Восстановление после удаления
	friend Node* SubTree(int, int, int*, Node*);  // Создание поддерева
	friend class Tree;
public:
	int data;		// данные в узле
	Node* L[2];		// левый сын, правый сын
	Node* parent;		// отец
	void Display(int, int, int = 1);	// Выдача узла на экран
	Node(int, Node*);		// Конструкторы
	Node();
};


Node* Node::root = 0;	// Объявление и инициализация статического поля

Node sentinel; 			//Лист - заглушка (-> конструктор заглушки)

Node* NIL = &sentinel;	// Ссылка на заглушку

/////////////////////////Итераторы/////////////////////////////////

// key == data
// friend class у Node - myiter

using MyStack = std::stack<std::pair<Node*, int>>;
struct myiter : public std::iterator<std::forward_iterator_tag, int> {
	Node* Ptr;   //Реальный указатель
	MyStack St;  //Стек с путём от корня дерева
	myiter(Node* p = NIL) : Ptr(p) { }
	myiter(Node* p, const MyStack&& St) : Ptr(p), St(move(St)) { }
	bool operator == (const myiter& Other) const { return Ptr == Other.Ptr; }
	bool operator != (const myiter& Other) const { return !(*this == Other); }
	myiter& operator++();
	myiter operator++(int) { myiter temp(*this); ++* this; return temp; }
	pointer operator->() { return &Ptr->data; }
	reference operator*() { return Ptr->data; }
	friend class Tree;
};

myiter& myiter::operator++()
{
	if (Ptr == NIL) { //Первое обращение?
		return *this; //Не работает без предварительной установки на дерево
	}
	if (Ptr->L[1] != NIL) {    // Шаг вправо
		St.push(std::make_pair(Ptr, 1));
		Ptr = Ptr->L[1];
		while (Ptr->L[0] != NIL) { //... и искать крайний левый
			St.push(std::make_pair(Ptr, 0));
			Ptr = Ptr->L[0];
		}
	}
	else {       // Подъём вверх, пока слева пусто
		std::pair<Node*, int> pp(Ptr, 1);
		while (!St.empty() && pp.second) { pp = St.top(); St.pop(); }
		if (pp.second) //Шаг вправо — тупик, завершить!
		{
			Ptr = NIL;
		}
		else Ptr = pp.first;  // Шаг вправо и продолжать
	}
	return (*this);
}


//Недоделано
//ИТЕРАТОР ВСТАВКИ (универсальный)
template <typename Container, typename Iter = myiter>
class outiter : public std::iterator<std::output_iterator_tag, typename Container::value_type>
{
protected:
	Container& container;    // Контейнер для вставки элементов
	Iter iter;                           // Текущее значение итератора чтения
public:
	outiter(Container& c, Iter it) : container(c), iter(it) { } // Конструктор
	const outiter<Container>& // Присваивание — вставка ключа в контейнер
		operator = (const typename Container::value_type& value) {
		iter = container.insert(value, iter).first;
		return *this;
	}
	const outiter<Container>& //Присваивание копии — фиктивное
		operator = (const outiter<Container>&) { return *this; }
	outiter<Container>& operator* () { return *this; } // Разыменование — пустая операция
	outiter<Container>& operator++ () { return *this; } // Инкремент — пустая операция
	outiter<Container>& operator++ (int) { return *this; }
};

// Функция для создания итератора вставки
template <typename Container, typename Iter>
inline outiter<Container, Iter> inserter(Container& c, Iter it)
{
	return outiter<Container, Iter>(c, it);
}
/////////////////////////Итераторы/////////////////////////////////


// Дерево в целом
class Tree {
private:
	Node* root;	// корень

//Конструкторы
public: 
	std::vector<int> seq; // Последовательность
	int firstrow=1;   // позиция на экране
	Tree() { root = NIL; }
	Tree(int count, int MAXint) : Tree()
	{
		for (int i = 0; i < count; ++i) { int key = rand() % MAXint; insert(key); seq.push_back(key); }
	}

	Tree(const Tree& rgt) : Tree()
	{	
		for (auto x : rgt) insert(x); 
		seq = rgt.seq; 
	}
	Tree(Tree&& rgt) : Tree() { swap(rgt); }
	Tree(std::vector<int> list) : Tree() {
		Tree temp;
		std::copy(list.begin(), list.end(), std::back_inserter(temp.seq));
		for (auto x : list) temp.insert(x);
		swap(temp);
	};
	~Tree();

public:
	void Display(char a, int ct);
	void Display(string s);
	void printSeq(string s="") { cout << s;  for (auto i : seq) std::cout << i << ' '; cout << "\n"; };
	Node* insertNode(int data);
	void deleteNode(Node* z);
	Node* findNode(int data);
	void BuildTree(int, int);
	void getData(vector<int>& data, Node* root)
	{
		data.push_back(root->data);
		if (root->L[0] != NIL) getData(data, root->L[0]);
		if (root->L[1] != NIL) getData(data, root->L[1]);
		sort(data.begin(), data.end());

	}
	vector<int> getRandomSeq()
	{
		vector<int> randSeq; int startFrom = rand() % seq.size();
		int size = rand() % (seq.size() - startFrom);
		for (int i = startFrom; i < startFrom + size; i++) randSeq.push_back(seq[i]);
		return randSeq;
	}

// Последовательность
public:
	void merge(const Tree& rgt);
	void concat(const Tree& rgt);
	void excl(const Tree& rgt);

public:
	using key_type = int;
	using value_type = int;
	using key_compare = std::less<int>;

public:
	Tree& operator = (Tree&& rgt) { swap(rgt); return *this; }
	Tree& operator |= (const Tree& rgt) {
		Tree temp;
		std::set_union(begin(), end(), rgt.begin(), rgt.end(), inserter(temp, myiter(NIL))); //Множество
		std::copy(temp.begin(), temp.end(), std::back_inserter(temp.seq)); //и последовательность к нему!
		swap(temp);
		return *this;
	}
	Tree operator | (const Tree& rgt) const
	{
		Tree result(*this); return (result |= rgt);
	}
	Tree& operator &= (const Tree& rgt) {
		Tree temp;
		std::set_intersection(begin(), end(), rgt.begin(), rgt.end(), inserter(temp, myiter(NIL))); //Множество
		std::copy(temp.begin(), temp.end(), std::back_inserter(temp.seq)); //и последовательность к нему!
		swap(temp);
		return *this;
	}
	Tree operator & (const Tree& rgt) const
	{
		Tree result(*this); return (result &= rgt);
	}
	Tree& operator -= (const Tree& rgt) {
		Tree temp;
		std::set_difference(begin(), end(), rgt.begin(), rgt.end(), inserter(temp, myiter(NIL))); //Множество
		std::copy(temp.begin(), temp.end(), std::back_inserter(temp.seq)); //и последовательность к нему!
		swap(temp);
		return *this;
	}
	Tree operator - (const Tree& rgt) const
	{
		Tree result(*this); return (result -= rgt);
	}
	Tree& operator ^= (const Tree& rgt) {
		Tree temp;
		std::set_symmetric_difference(begin(), end(), rgt.begin(), rgt.end(), inserter(temp, myiter(NIL))); //Множество
		std::copy(temp.begin(), temp.end(), std::back_inserter(temp.seq)); //и последовательность к нему!
		swap(temp);
		return *this;
	}
	Tree operator ^ (const Tree& rgt) const
	{
		Tree result(*this); return (result ^= rgt);
	}

	void swap(Tree& rgt) //Обмен содержимого двух ДДП
	{
		std::swap(firstrow, rgt.firstrow); std::swap(root, rgt.root);
		std::swap(seq, rgt.seq);
	}

	myiter begin() const
	{
		MyStack St;
		Node* p(root);
		if (p != NIL) {   //Идём по левой ветви, запоминая путь в стеке
			while (p->L[0] != NIL) {
				St.push(std::make_pair(p, 0));
				p = p->L[0];
			}
		}
		return myiter(p, move(St)); //Создаём итератор, передаём ему стек
	};
	myiter end() const { return myiter(NIL); }  // ВОЗМОЖНО NIL

	std::pair<myiter, bool> insert(int k, myiter where = NIL);//Если не указан итератор - свободная вставка

};

//Вставка узла
std::pair<myiter, bool> Tree::insert(int k, myiter where)
{
	MyStack St;
	Node* adr = insertNode(k);
	//seq.push_back(k);
	return make_pair(myiter(adr, move(St)), true);

}

// Соединить 2 последовательности (с сортировкой)
void Tree::merge(const Tree& rgt)
{
	Tree temp;

	// последовательность (меняется порядок вхождения эл-в)
	vector<int>sorted_t1; vector<int>sorted_t2;
	std::copy(seq.begin(), seq.end(), std::back_inserter(sorted_t1));
	std::copy(rgt.seq.begin(), rgt.seq.end(), std::back_inserter(sorted_t2));
	std::sort(sorted_t1.begin(), sorted_t1.end());
	std::sort(sorted_t2.begin(), sorted_t2.end());
	std::merge(sorted_t1.begin(), sorted_t1.end(),	sorted_t2.begin(), sorted_t2.end(), 
		back_inserter(temp.seq));

	// множество
	for (auto x : temp.seq) temp.insert(x);

	swap(temp);
}

// Соединить 2 последовательности. Вторая идет за первой.
void Tree::concat(const Tree& rgt)
{
	Tree temp;

	// последовательность (не меняется порядок вхождения эл-в)
	std::copy(seq.begin(), seq.end(), std::back_inserter(temp.seq));
	temp.seq.insert(temp.seq.end(), rgt.seq.begin(), rgt.seq.end());

	// множество
	for (auto x : temp.seq) temp.insert(x);

	swap(temp);
}

/*
// Исключить из последовательности подпоследовательность
void Tree::excl(const Tree& rgt)
{
	Tree temp;
	// последовательность (меняется порядок вхождения эл-в)
	vector<int>sorted_t1; vector<int>sorted_t2;
	std::copy(seq.begin(), seq.end(), std::back_inserter(sorted_t1));
	std::copy(rgt.seq.begin(), rgt.seq.end(), std::back_inserter(sorted_t2));
	std::sort(sorted_t1.begin(), sorted_t1.end());
	std::sort(sorted_t2.begin(), sorted_t2.end());
	std::set_difference(sorted_t1.begin(), sorted_t1.end(), sorted_t2.begin(), sorted_t2.end(),
		std::back_inserter(temp.seq));

	// множество
	for (auto x : temp.seq) temp.insert(x);

	swap(temp);
}

*/

/*
void Tree::excl(const Tree& rgt)
{
	Tree temp; int count{ 0 };

	auto iter = search(seq.cbegin(), seq.cend(), rgt.seq.cbegin(), rgt.seq.cend(), 
		[](decltype(seq.cbegin())x, decltype(seq.cbegin())y) { return (x == y); }); // Поиск подпоследовательности
	
	//cout << *iter;

	/*
	if (iter != seq.cend()) {   // Найдена. Удаляем
		int p{ iter – seq.cbegin() };
		int q{iter}
	};

	copy_if(cbegin(), cend(), inserter(temp),	[&count]() {++count; return(count<p || count>q); });
	
	//swap(temp);

}
*/
// Исключить из последовательности подпоследовательность
void Tree::excl(const Tree& rgt)
{
	Tree temp;
	vector<int>::iterator result;

	result = std::find_end(seq.begin(), seq.end(), rgt.seq.begin(), rgt.seq.end());
	int p = std::distance(seq.begin(), result);
	int q = std::distance(rgt.seq.begin(), rgt.seq.end());
	q = p + q;

	int count=-1;
	copy_if(seq.cbegin(), seq.cend(), std::back_inserter(temp.seq),
		[&p, &q, &count](int x) 
		{
			count++;		
			return(count<p || count>=q);
		});
	
	swap(temp);

}


Node::Node(int d, Node* p) :	//Конструктор нового узла
	data(d), parent(p), color(RED)
{
	L[0] = NIL;
	L[1] = NIL;
}

Node::Node(void) :		//Конструктор заглушки
	data(0), parent(nullptr), color(BLACK)
{
	L[0] = this;
	L[1] = this;
}

void Node::Destroy()	// Удаление дерева
{
	if (this != NIL)
	{
		L[0]->Destroy();
		L[1]->Destroy();
		delete this;
	}
}

Tree :: ~Tree()
{
	if (root != NIL) root->Destroy();
}

const int FIRSTROW = 0, SECONDROW = 10, FIRSTCOL = 48, //Параметры вывода на экран
OFFSET[] = { 50, 25, 12, 6, 3, 2, 1 }, MAXCOL = 100,
MAXROW = 9, // FIRSTROW + 9,
MAXDEPTH = 6,
MAXOUT = FIRSTROW + 6, SHIFT = 2;
string SCREEN[MAXROW];
//int row = 0, col = 0;
//void gotoxy(int c, int r) { row = r, col = c; }

void clrscr()	// Очистка буфера выдачи дерева и экрана
{
	for (auto i = 0; i < MAXROW; ++i) {
		SCREEN[i].clear();
		SCREEN[i].resize(MAXCOL + 20, '.');
	}
	//system("cls");
}

void showscr()	// Выдача буфера на экран
{
	for (auto i = 0; i < MAXROW; ++i) {
		SCREEN[i].resize(MAXCOL, '.');
		cout << SCREEN[i] << '\n';
	}
}

int setval(string& s, int pos, int val, nodeColor color) {
	char CL[] = " *";
	string t(to_string(val) + CL[color]);
	for (auto p : t) s[pos++] = p;
	return t.size();
}

void Tree::Display(char a, int ct)		// Вывод дерева в целом
{
	int col = FIRSTCOL, row = firstrow;
	if (!firstrow) clrscr();
	SCREEN[firstrow] = "Ждём " + to_string(a) + "; осталось " + to_string(ct) + " --- RB-Tree --->";
	if (root != NIL) {
		SCREEN[firstrow].resize(MAXCOL, '.');
		root->Display(firstrow, FIRSTCOL, 1);
	}
	else SCREEN[firstrow] += "<Пусто!>";
	if (firstrow) showscr();
}

void Tree::Display(string s = "")		// Вывод дерева в целом
{
	int col = FIRSTCOL, row = firstrow;
	if (!firstrow) clrscr();
	SCREEN[firstrow] = s;
	if (root != NIL) {
		SCREEN[firstrow].resize(MAXCOL, '.');
		root->Display(firstrow, FIRSTCOL, 1);
	}
	else SCREEN[firstrow] += "<Пусто!>";
	if (firstrow) showscr();
}

void Node::Display(int row, int col, int depth)	// Вывод одного узла
{
	char CL[] = " *";
	if ((row > MAXROW) || (col < 0) || (col > MAXCOL)) return;
	//	if (row > MAXOUT) {
	//		SCREEN[row].replace(col, 3, "+++");
	//		return;
	//	}
		//	SCREEN[row].replace(col, col+5, to_string(key));
	try {
		setval(SCREEN[row], col, data, color);
		//setval(SCREEN[row + 1], col + 1, h);
	}
	catch (exception& e) {
		cout << e.what() << data << static_cast<char>(CL[color]) << row << ' ' << col << endl;
		cin.get();
	}
	catch (...) { cout << "Unknown error\n"; cin.get(); }
	if (L[0] != NIL) L[0]->Display(row + 1, col - OFFSET[depth], depth + 1);
	if (L[1] != NIL) L[1]->Display(row + 1, col + OFFSET[depth], depth + 1);
}

void Node::rotate(Direction D)  //Поворот в направлении D (комментарии для случая "влево")
{
	Node* x = this, * y = x->L[1 - D];  //=right

	/* установка указателя x->right */
	x->L[1 - D] = y->L[D];
	if (y->L[D] != NIL) y->L[D]->parent = x;

	/* установка указателя y->parent */
	if (y != NIL) y->parent = x->parent;
	if (x->parent) {
		if (x == x->parent->L[D])
			x->parent->L[D] = y;
		else
			x->parent->L[1 - D] = y;
		//           x->parent->L[1 - (x == x->parent->L[D])] = y;
	}
	else root = y;

	/* связывание x и y */
	y->L[D] = x;
	if (x != NIL) x->parent = y;
}

void Node::insertFixup() {	// Балансировка после вставки

	Node* x = this;
	Direction D;
	/* проверка красно-чёрных свойств */
	while (x != root && x->parent->color == RED) {
		/* обнаружено нарушение... */
		D = (Direction)(x->parent == x->parent->parent->L[0]);

		Node* y = x->parent->parent->L[D];  //right
		if (y->color == RED) {

			/* дядя - RED */
			x->parent->color = BLACK;
			y->color = BLACK;
			x->parent->parent->color = RED;
			x = x->parent->parent;
		}
		else {

			/* дядя - BLACK */
			if (x == x->parent->L[D]) { //right
				 /* делаем x левым сыном */
				x = x->parent;
				x->rotate((Direction)(1 - D));
			}

			/* перекраска и поворот */
			x->parent->color = BLACK;
			x->parent->parent->color = RED;
			//	 rotateRight(x->parent->parent);
			x->parent->parent->rotate(D);
		}
	}
	root->color = BLACK; //Корень должен быть всегда чёрный!
}

Node* Tree::insertNode(int data) {// Вставка ключа с возвратом его адреса

	Node* current, * parent, * x;
	Node::root = this->root;
	// поиск места вставки... 
	current = root;
	parent = 0;
	while (current != NIL) {
		if (data == current->data) return (current);
		parent = current;	// Ключ уже имеется, ВЫХОД
		current = (data < current->data) ?
			current->L[0] : current->L[1];
	}
	// создание нового узла и вставка в дерево
	x = new Node(data, parent);
	if (parent) {
		if (data < parent->data)
			parent->L[0] = x;
		else
			parent->L[1] = x;
	}
	else Node::root = root = x; // Запоминание корня
	x->insertFixup();	// Коррекция формы дерева
	root = Node::root;	// Восстановление корня
	return x;
}

void Node::deleteFixup()	// Коррекция после удаления 
{
	Node* x = this;
	while (x != root && x->color == BLACK) {
		Direction D = (Direction)(x == x->parent->L[0]);
		Node* w = x->parent->L[D];		//right
		if (w->color == RED) {
			w->color = BLACK;
			x->parent->color = RED;
			x->parent->rotate((Direction)(1 - D));	//left
			w = x->parent->L[D];		//right
		}
		if (w->L[0]->color == BLACK && w->L[1]->color == BLACK) {
			w->color = RED;
			x = x->parent;
		}
		else {
			if (w->L[D]->color == BLACK) {	//right
				w->L[1 - D]->color = BLACK;	//left
				w->color = RED;
				w->rotate(D);			//right
				w = x->parent->L[D];		//right
			}
			w->color = x->parent->color;
			x->parent->color = BLACK;
			w->L[D]->color = BLACK;		//right
			x->parent->rotate((Direction)(1 - D));		//left
			x = root;
		}
	}
	x->color = BLACK;
}

void Tree::deleteNode(Node* z) // Удаление узла с адресом z из дерева
{
	Node* x, * y;
	Node::root = this->root; // Запомнить корень

	if (!z || z == NIL) return; // пусто или NIL-узел -> ВЫХОД
	if (z->L[0] == NIL || z->L[1] == NIL) {
		/* узел z имеет сына - NIL-узел, можно удалить/исключить z */
		y = z;
	}
	else {
		/* поиск замещающего узла с NIL-сыном */
		y = z->L[1];
		while (y->L[0] != NIL) y = y->L[0];
	}

	/* x - единственный сын узла y */
	if (y->L[0] != NIL)
		x = y->L[0];
	else
		x = y->L[1];

	/* удаление y из цепочки от отца */
	x->parent = y->parent;
	if (y->parent)
		if (y == y->parent->L[0])
			y->parent->L[0] = x;
		else
			y->parent->L[1] = x;
	else
		root = x;

	if (y != z) z->data = y->data;

	if (y->color == BLACK) //Удаляется чёрный НЕ лист...
		x->deleteFixup();

	delete y;
	root = Node::root;	// Восстановить корень
}
Node* Tree::findNode(int data) // Поиск узла по ключу
{
	Node* current = root;
	while (current != NIL) {
		if (data == current->data) return (current); // Нашли
		else
			current = (data < current->data) ?
			current->L[0] : current->L[1];
	}
	return(0); // не нашли
}

Node* SubTree(int a, int b, int* source, Node* parent)
{
	if (b < a) return NIL;
	else if (b == a) return new Node(source[a], parent);
	else {
		int c = a + (int)((b - a) / 2);
		Node* t = new Node(source[c], parent);
		t->L[0] = SubTree(a, c - 1, source, t);
		t->L[1] = SubTree(c + 1, b, source, t);
		t->color = BLACK;	// делаем все узлы чёрными
		return t;
	}
}

void Tree::BuildTree(int power, int Upower)
{
	int* sourse = new int[power], d = Upower / power, s = d;
	for (int i = 0; i < power; i++)
		sourse[i] = s += rand() % d + 1;
	root = SubTree(0, power - 1, sourse, nullptr);
	getData(seq, root);
	delete[]sourse;
}