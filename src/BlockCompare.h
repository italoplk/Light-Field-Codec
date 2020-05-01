#if !defined(__BLOCKCOMPARATOR_H_)
#define __BLOCKCOMPARATOR_H_

template <typename T> class BlockComparator {

public:
  /**
   * Method to compare two blocks.
   * It relies on comparing features and statistical data
   * of each parameter. At the end, it returns a notion of
   * which block has more energy than the other.
   *
   * @param lhs: ponter to the left block.
   * @param rhs: pointer to the right block.
   * @return
   *     -1 if lhs is smaller than rhs
   *      0 if lhs equal rhs
   *      1 if lhs is bigger than rhs
   *
   */
  int compare(const T *lhs, const T *rhs);
};

template <typename T>
int BlockComparator<T>::compare(const T *lhs, const T *rhs) {
  // Código aqui.
}

#endif // __BLOCKCOMPARATOR_H_
