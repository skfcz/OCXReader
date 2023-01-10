#pragma once

#include <stdexcept>

/**
 * All OCCUtils exceptions derive from this
 */
struct OCCUtilsBaseException : std::invalid_argument {
  using invalid_argument::invalid_argument;
};

/**
 * Baseclass for exceptions that directly relate to the
 * BRep topology.
 */
struct OCCConstructionFailedException : OCCUtilsBaseException {
  using OCCUtilsBaseException::OCCUtilsBaseException;
};

/**
 * Exception occurring related to IO of data
 */
struct OCCIOException : OCCUtilsBaseException {
  using OCCUtilsBaseException::OCCUtilsBaseException;
};

/**
 * Baseclass for exceptions that directly relate to the
 * BRep topology.
 */
struct OCCTopologyException : OCCUtilsBaseException {
  using OCCUtilsBaseException::OCCUtilsBaseException;
};

/**
 * Thrown if there are either too many or too few
 * sub-topologies in a topology,
 * e.g. too many or too few edges in a face.
 * "Too many" or "Too few" is relative to what the calling code expects.
 */
struct OCCTopologyCountMismatchException : OCCTopologyException {
  using OCCTopologyException::OCCTopologyException;
};
