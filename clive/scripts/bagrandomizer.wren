import "random" for Random

class BagRandomizer {
   construct new(itemCount) {
      _itemCount = itemCount
      _rnd = Random.new()
      _list = []
      reset()
   }

   reset() {
      _list = (0..._itemCount).toList
      _rnd.shuffle(_list)
   }

   next() {
      if (_list.count == 0) {
         reset()
      }

      return _list.removeAt(-1)
   }
}