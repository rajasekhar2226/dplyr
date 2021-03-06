#ifndef dplyr_DataFrameJoinVisitors_H
#define dplyr_DataFrameJoinVisitors_H

#include <tools/pointer_vector.h>

#include <dplyr/visitor_set/VisitorSetMixin.h>

#include <dplyr/tbl_cpp.h>
#include <dplyr/JoinVisitor.h>

namespace dplyr {

  class DataFrameJoinVisitors :
    public VisitorSetEqual<DataFrameJoinVisitors>,
    public VisitorSetHash<DataFrameJoinVisitors>
  {
  public:
    typedef JoinVisitor visitor_type;

    DataFrameJoinVisitors(
      const Rcpp::DataFrame& left_,
      const Rcpp::DataFrame& right_,
      Rcpp::CharacterVector names_left,
      Rcpp::CharacterVector names_right,
      bool warn_,
      bool na_match
    );

    inline JoinVisitor* get(int k) const {
      return visitors[k];
    }
    inline JoinVisitor* get(String name) const {
      for (int i=0; i<nvisitors; i++) {
        if (name == visitor_names_left[i]) return get(i);
      }
      stop("visitor not found for name '%s' ", name.get_cstring());
    }
    inline int size() const {
      return nvisitors;
    }

    template <typename Container>
    inline DataFrame subset(const Container& index, const CharacterVector& classes) {
      int nrows = index.size();
      Rcpp::List out(nvisitors);
      for (int k=0; k<nvisitors; k++) {
        out[k] = get(k)->subset(index);
      }
      set_class(out, classes);
      set_rownames(out, nrows);
      out.names() = visitor_names_left;
      copy_vars(out, left);
      return (SEXP)out;
    }

    const CharacterVector& left_names() const {
      return visitor_names_left;
    }
    const CharacterVector& right_names() const {
      return visitor_names_right;
    }

  private:
    const DataFrame& left;
    const DataFrame& right;
    CharacterVector visitor_names_left;
    CharacterVector visitor_names_right;

    int nvisitors;
    pointer_vector<JoinVisitor> visitors;
    bool warn;

  };

}

#endif
