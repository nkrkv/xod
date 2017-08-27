
module Data.Pin where

type PinKey = String

data DataType = Pulse | Boolean | Number | String

type DataValue = String

data Direction = Input | Output

type Pin =
    { key :: PinKey
    , dataType :: DataType
    , direction :: Direction
    , label :: String
    , description :: String
    , order :: Int
    , isBindable :: Boolean
    , defaultValue :: DataValue
    }

createPin :: PinKey ->
             DataType ->
             Direction ->
             String ->
             String ->
             Int ->
             Boolean ->
             DataValue ->
             Pin
createPin key dataType direction label description order isBindable defaultValue
    = { key, dataType, direction, label, description, order, isBindable, defaultValue }

getPinType :: Pin -> DataType
getPinType pin = pin.dataType

getPinDirection :: Pin -> Direction
getPinDirection pin = pin.direction

getPinKey :: Pin -> PinKey
getPinKey pin = pin.key

getPinLabel :: Pin -> String
getPinLabel pin = pin.label

getPinDescription :: Pin -> String
getPinDescription pin = pin.description

getPinDefaultValue :: Pin -> DataValue
getPinDefaultValue pin = pin.defaultValue

getPinOrder :: Pin -> Int
getPinOrder pin = pin.order

isPinBindable :: Pin -> Boolean
isPinBindable pin = pin.isBindable

isInputPin :: Pin -> Boolean
isInputPin pin = case getPinDirection pin of
                      Input -> true
                      _ -> false

isOutputPin :: Pin -> Boolean
isOutputPin pin = case getPinDirection pin of
                       Output -> true
                       _ -> false
