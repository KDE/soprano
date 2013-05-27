#
# Macro soprano_add_ontology adds source files generated via Soprano's onto2vocabularyclass tool
# to the specified sources list.
# The files are generated in the current binary dir
#
# soprano_add_ontology( SOURCES
#            ONTOLOGY_FILE
#            ONTOLOGY_NAME
#            NAMESPACE
#            ENCODING
#            [VISIBILITY VISIBILITY_NAME]
# )
#
# As an optional argument one can add VISIBILITY <visibility-name>
#
# Example:
#  soprano_add_ontology(pimo_LIB_SRCS ${PIMO_TRIG_SOURCE} "PIMO" "Nepomuk::Vocabulary" "trig" VISIBILITY "nepomuk")
#
# Copyright (C) 2009-2010 Sebastian Trueg <trueg@kde.org>
#

MACRO(SOPRANO_ADD_ONTOLOGY _sources _ontoFile _ontoName _namespace _encoding)
  # extract optional visibility argument
  set(_args ${ARGN})
  list(GET _args 0 _arg0)
  list(GET _args 1 _arg1)

  if(${_arg0} STREQUAL "VISIBILITY")
    set(_visibility "--export-module" ${_arg1})
  else()
    set(_visibility "--no-visibility-export")
  endif()

  string(TOLOWER ${_ontoName} _ontoFilePrefix)
  set(_ontoHeaderFile "${CMAKE_CURRENT_BINARY_DIR}/${_ontoFilePrefix}.h")
  set(_ontoSourceFile "${CMAKE_CURRENT_BINARY_DIR}/${_ontoFilePrefix}.cpp")

  # SOPRANO_ONTO2VOCABULARYCLASS_EXECUTABLE exist in FindSoprano.cmake from kdelibs since Nov 12 2009
  # Just set it to the name of the executable (without path) for the case that this file is used with
  # an older kdelibs:
  if (NOT SOPRANO_ONTO2VOCABULARYCLASS_EXECUTABLE)
     set(SOPRANO_ONTO2VOCABULARYCLASS_EXECUTABLE onto2vocabularyclass)
  endif()
  add_custom_command(OUTPUT ${_ontoHeaderFile} ${_ontoSourceFile}
    COMMAND ${SOPRANO_ONTO2VOCABULARYCLASS_EXECUTABLE} --name ${_ontoName} --encoding ${_encoding} --namespace ${_namespace} ${_visibility} ${_ontoFile}
    MAIN_DEPENDENCY ${_ontoFile}
    DEPENDS ${_ontoFile}
    )

  list(APPEND ${_sources} ${_ontoSourceFile})

  # reset the variables we used
  unset(_visibility)
  unset(_ontoName)
  unset(_ontoFilePrefix)
  unset(_ontoHeaderFile)
  unset(_ontoSourceFile)
ENDMACRO(SOPRANO_ADD_ONTOLOGY)
