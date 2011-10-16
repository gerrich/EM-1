set(NAME parser)

add_executable(${NAME}
  parser.cpp
)

#======================

set(NAME sorter)

add_executable(${NAME}
  sorter.cpp
)

#======================

set(NAME biller)

add_executable(${NAME}
  biller.cpp
)

#======================

add_executable(em_copy
  em_copy.cpp
)
add_executable(em_test
  em_test.cpp
)
add_executable(door_test
  door_test.cpp
)

#======================


