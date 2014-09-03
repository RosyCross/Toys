#ifndef INSERTER_H_
#define INSERTER_H_

#include<iterator>

//namespace InserterSpace
//{

template<typename Container>
class Inserter 
{
	public:
		typedef typename Container::iterator iterator;
		
	public:
		Inserter(Container& userContainer) : container_(userContainer) {}		
		void operator()(const typename Container::value_type& input);
		template<typename T>
		void operator()(const T& input);
        Container& getContainer() { return container_;};        
		
	private:
		Container& container_;
};

//} //namespace end
#endif



