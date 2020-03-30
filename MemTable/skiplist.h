#pragma once

#include "Quadlist.h"
#include "list.h"

#define prob 0.5 //��������
#define MAX_SIZE 2*1024*1024

class Skiplist: public List<Quadlist*>
{
private:
	int fileSize = 0;
protected:
	bool skipSearch(ListNode<Quadlist*>* &l, QuadlistNode* &n, K& k);
public:
	Skiplist() {};
	~Skiplist() {};
	int size() const { return empty() ? 0 : back()->data->size(); }
	int level() { return size(); }
	bool put(const K, const V);
	V* get(K key);
	bool remove(K key);
	QuadlistNode* getFirst() { return back()->data->front(); }
	QuadlistNode* getLast() { return back()->data->back(); }
	bool full() { return fileSize > MAX_SIZE; }
	void reset();
};

inline void Skiplist::reset() {
	fileSize = 0;
	clear();
}

inline bool Skiplist::skipSearch(ListNode<Quadlist*>* &qlist, QuadlistNode* &p, K& k) {
	while (true) {
		while (p->succ && (p->entry.key <= k)) {
			p = p->succ;
		}//ֱ����������key����p=tailor
		p = p->pred;
		if (p->pred && (k == p->entry.key))return true;//����
		qlist = qlist->succ;//ת����һ��
		if (!qlist->succ)return false;//�ѵ��ײ�
		if (p->pred) p = p->below;
		else p = qlist->data->front();//ת����ǰ������һ�ڵ�
	}
}

inline bool Skiplist::put(K k, V v)
{
	if (full()) return false;//skiplist���˲�����
	Entry e = Entry(k,v);
	if (empty())
		pushFront(new Quadlist());
	ListNode<Quadlist*>* qlist = front();
	QuadlistNode* p = qlist->data->front();
	if (skipSearch(qlist, p, k)) {//���Һ��ʵĲ���λ�ã������ڹؼ���k�����һ���ڵ�p
		while (p->below) {
			p = p->below;//ǿ��ת������		
		}
		while (p) {
			p->entry.value = v;
			p = p->above;
		}
		return true;
	}
	qlist = back();
	QuadlistNode* newList = qlist->data->insertAfterAbove(e, p);
	while (rand() & 1) {
		while (qlist->data->valid(p) && !p->above)p = p->pred;
		if (!qlist->data->valid(p)) {//����ǰ����header
			if (qlist == front()) {//��ǰ�������
				pushFront(new Quadlist());//���ȴ����µ�һ��
			}
			p = qlist->pred->data->front()->pred;//pת����һ��skiplist��header
		}
		else {
			p = p->above;
		}
		qlist = qlist->pred;
		newList = qlist->data->insertAfterAbove(e, p, newList);//�����½ڵ�
	}
	fileSize += v.size() + 16;
	return true;
}

inline V * Skiplist::get(K key)
{
	if (empty()) return nullptr;
	ListNode<Quadlist*>* qlist = front();//����Quadlist��
	QuadlistNode* p = qlist->data->front();//��һ���ڵ㿪ʼ
	if (skipSearch(qlist, p, key)) {
		return p->entry.getValue();
	}
	else return nullptr;
}

inline bool Skiplist::remove(K key)
{
	if (empty())return false;
	ListNode<Quadlist*>* qlist = front();//�Ӷ����������
	QuadlistNode* p = qlist->data->front();//�׽ڵ����
	if (!skipSearch(qlist, p, key))return false;//Ŀ����������ڣ�ֱ�ӷ���
	do {
		QuadlistNode* lower = p->below;
		qlist->data->remove(p);//ɾ����ǰ��ڵ�
		p = lower; qlist = qlist->succ;//ת����һ��
	} while (qlist->succ);//ֱ������
	while (!empty() && front()->data->empty()) {
		List::remove(front());//������ܲ��������Ķ���Quadlist
	}
	return true;
}
