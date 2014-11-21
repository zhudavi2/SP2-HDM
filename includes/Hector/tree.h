#pragma once
#ifndef _TREE_
#define _TREE_

namespace std
{
	template<class T> class treenode;
	template<class T> class treenodes;

	template<class T> class Enumerator
	{
	private:
		friend treenodes<T>;
		Enumerator(typename treenodes<T>::iterator beg, typename treenodes<T>::iterator end)
		: m_Itr(beg), m_End(end)
		{
			m_Itr--;
		}

	protected:
      typename treenodes<T>::iterator m_Itr;
		typename treenodes<T>::iterator m_End;
	public:

		bool MoveNext()
		{
			if (m_Itr >= m_End) return false;
			return ++m_Itr != m_End;
		}

      bool IsLast() const
      {
         return m_Itr == m_End-1;
      }

		treenode<T> * Current()
		{
			return *m_Itr;
		}
	};


	template<class T> class treenodes : protected vector <treenode<T>*>
	{
	private:
		friend treenode<T>;
      friend Enumerator<T>;
		treenodes(treenode<T>* node) {
         m_node = node;
      }
      treenodes(){;}

		treenode<T>* m_node;

		treenodes(const treenodes &) {;}
		operator = (const treenodes &) {;}
	public:
		typedef typename std::Enumerator<T> Enumerator;

		std::Enumerator<T> GetEnumerator()
		{
         return Enumerator(begin(), end());
		}

		void Add(treenode<T>* node, treenode<T>* InsertBefore = NULL)
		{
			if (InsertBefore)
			{
				for (treenodes::iterator i = this->begin(); i != this->end(); i++)
					if (*i == InsertBefore)
					{
						this->insert(i, node);
						break;
					}
			}
			else
				this->push_back(node);

			m_node->modified();
		}

		void Remove(value_type& item)
		{
			for (treenodes::iterator i = this->begin(); i != this->end(); i++)
				if (*i == item)
				{
					this->erase(i);
					m_node->modified();
					return;
				}
		}

		void RemoveAt(difference_type idx)
		{
			this->erase(this->begin() + idx);
			m_node->modified();
		};

		void Clear()
		{
			this->clear();
			m_node->modified();
		}

		reference operator [](difference_type idx)
		{
			return vector<treenode<T>*>::operator[](idx);
		}

		size_type Count()
		{
			return this->size();
		}
	};

	template<class T> class treenode
	{
	private:
      treenode() : m_Nodes(this) {;}
		treenode(const treenode &) {;}
		void operator = (const treenode &) {;}

	protected:
		treenode(T item) : m_Nodes(this)
		{
			m_Item = item;
		}

		friend treenodes<T>;
		virtual void modified()=0;
		T m_Item;
		treenodes<T> m_Nodes;

	public:
		T Item() {return m_Item;};
		treenodes<T> & Nodes() {return m_Nodes;};
	};
}

#endif