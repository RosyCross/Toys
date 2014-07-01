#ifndef INSERTER_H_
#define INSERTER_H_

#include<iterator>
template<typename Container>

class Inserter /*: std::iterator_traits<typename Container::iterator>*/
{
	public:
		typedef typename Container::iterator iterator;
		
	public:
		Inserter(Container& userContainer) : container_(userContainer) {}		
		void operator()(const typename Container::value_type& input);
        Container& getContainer() { return container_;};        
		
	private:
		Container& container_;
};
#endif

