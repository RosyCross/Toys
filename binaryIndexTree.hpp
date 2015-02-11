#ifndef BINARYINDEXTREE_HPP_
#define BINARYINDEXTREE_HPP_

#ifndef _STDDEF_H_
#include <stddef.h>
#endif
#include <stdexcept>

#define OUT_OF_BOUND_CHECK(currIdx,msg) if ( currIdx > lastIdx() || currIdx < firstIdx() ) \
{ throw std::range_error(msg); }
 
template<typename ValueType>
class BIT
{
    public:
        BIT(size_t size,const ValueType& defaultValue):
        size_(size), array_(new ValueType[size])
        {
            for (size_t idx = 0; idx <= size; ++idx) array_[idx] = defaultValue;
        }
       ~BIT() 
        {
            if ( NULL == array_ ) return;
            delete[] array_;
            array_ = NULL;
        }

        size_t getSize()  const { return size_; }
        size_t firstIdx() const { return 1;};
        size_t lastIdx()  const { return size_;}
        //
        // Beware: getNextIdx and getPrevIdx are not two complementary function
        //         that means idx != getPrevIdx( getNextIdx(idx) )
        //
        size_t getNextIdx(size_t currIdx) const 
               { return currIdx += currIdx & -currIdx; }
        size_t getPrevIdx(size_t currIdx) const 
               { return currIdx -= currIdx & -currIdx; }

        //canonical semantics of get value at certain index
        const ValueType getValue(size_t idx) const
        {
            OUT_OF_BOUND_CHECK(idx,"BIT::getValue: out of bound access");
            ValueType rslt = ValueType(); // default value
            while ( firstIdx() <= idx  )
            {
                rslt += array_[idx];
                idx = getPrevIdx(idx);
            }
            return rslt;
        }

        const ValueType& getValueAt(size_t currIdx) const
        { 
            if ( currIdx > lastIdx() || currIdx < firstIdx() ) 
            { throw std::range_error("BIT::getValueAt: out_of_bound access"); }

            return array_[currIdx]; 
        }
        ValueType& getValueAt(size_t currIdx)
        {
            return const_cast<ValueType&>( 
                   static_cast<const BIT&>(*this).getValueAt(currIdx)
                   );
        }

        const ValueType& setValueAt(size_t currIdx, const ValueType& value)
        {
            if ( currIdx > lastIdx() || currIdx < firstIdx() ) 
            { throw std::range_error("BIT::setValueAt: out_of_bound access"); }

            return (array_[currIdx] = value);
        }

    private:

        ValueType* getArray() { return array_; }
        size_t size_;
        ValueType* array_;
};

#endif
