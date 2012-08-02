//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   IO_HDF5.cc
 * \brief  IO_HDF5 
 * \author Jeremy Roberts
 * \date   Jul 29, 2012
 */
//---------------------------------------------------------------------------//

// Configuration
#include "detran_config.h"

#ifdef DETRAN_ENABLE_HDF5

// Detran IO Utils
#include "IO_HDF5.hh"

// System
#include <sstream>

namespace detran_ioutils
{

IO_HDF5::IO_HDF5(std::string filename)
  : d_filename(filename)
  , d_open(false)
{
  /* ... */
}

void IO_HDF5::open()
{
  // Open the HDF5 file
  d_file_id = H5Fcreate(d_filename.c_str(), // filename
                        H5F_ACC_TRUNC,      // overwrite existing file
                        H5P_DEFAULT,        // file create property list
                        H5P_DEFAULT);       // file access property list
  d_open = true;
}

void IO_HDF5::write(SP_input input)
{
  // Preconditions
  Require(input);

  if (!d_open) open();

  // Create the input group
  hid_t group = H5Gcreate(d_file_id, "/input",
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  // HDF5 error return value
  herr_t  status;

  // Create variable string type
  d_string_type = H5Tcopy (H5T_C_S1);
  status = H5Tset_size (d_string_type, H5T_VARIABLE);

  //-------------------------------------------------------------------------//
  // PREPARE DATA
  //-------------------------------------------------------------------------//

  // Get the map sizes for the suppored types.
  int size_int      = input->size(detran::InputDB::INT);
  int size_dbl      = input->size(detran::InputDB::DBL);
  int size_str      = input->size(detran::InputDB::STR);
  int size_vec_int  = input->size(detran::InputDB::VEC_INT);
  int size_vec_dbl  = input->size(detran::InputDB::VEC_DBL);

  // Define the temporary data containers.
  compound_type<int>            data_int[size_int];
  compound_type<double>         data_dbl[size_dbl];
  compound_type<std::string>    data_str[size_str];
  compound_type<vec_int>        data_vec_int[size_vec_int];
  compound_type<vec_dbl>        data_vec_dbl[size_vec_dbl];

  //-------------------------------------------------------------------------//
  // WRITE DATA
  //-------------------------------------------------------------------------//

  write_data(input, group, "int_data", data_int);
  write_data(input, group, "dbl_data", data_dbl);
  write_data(input, group, "str_data", data_str);
  write_data(input, group, "vec_int_data", data_vec_int);
  write_data(input, group, "vec_dbl_data", data_vec_dbl);

  // Close the group.
  status = H5Gclose(group);

}

void IO_HDF5::write(SP_material mat)
{
  // Preconditions
  Require(mat);

  if (!d_open) open();

  // Create the material group
  hid_t group = H5Gcreate(d_file_id, "/material",
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  herr_t status;

  //-------------------------------------------------------------------------//
  // ATTRIBUTES
  //-------------------------------------------------------------------------//

  hid_t att_space;
  hid_t att;

  // Create scalar attribute.
  att_space = H5Screate(H5S_SCALAR);
  att = H5Acreate(group, "number_groups", H5T_NATIVE_INT, att_space, H5P_DEFAULT, H5P_DEFAULT);
  // Write scalar attribute.
  int ng = mat->number_groups();
  status = H5Awrite(att, H5T_NATIVE_INT, (void *) &ng);
  // Close dataspace and attribute
  status = H5Sclose(att_space);
  status = H5Aclose(att);

  // Create scalar attribute.
  att_space = H5Screate(H5S_SCALAR);
  att = H5Acreate(group, "number_materials", H5T_NATIVE_INT, att_space, H5P_DEFAULT, H5P_DEFAULT);
  // Write scalar attribute.
  int nm = mat->number_materials();
  status = H5Awrite(att, H5T_NATIVE_INT, (void *) &nm);
  // Close dataspace and attribute
  status = H5Sclose(att_space);
  status = H5Aclose(att);

  //-------------------------------------------------------------------------//
  // DATA
  //-------------------------------------------------------------------------//

  hid_t dset;
  hid_t space;

  for (int m = 0; m < nm; m++)
  {
    // Create material name
    std::string name = "material";
    std::ostringstream convert;
    convert << m;
    name += convert.str();

    // Create the material group
    hid_t group_m = H5Gcreate(group, name.c_str(),
                              H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);


    // All data is ng long, except scatter, which is ng * ng
    hsize_t dims1[1] = {ng};
    hsize_t dims2[2] = {ng, ng};

    // TOTAL, FISSION, NU, CHI, DIFFUSION
    space  = H5Screate_simple(1, dims1, NULL);

    dset   = H5Dcreate(group_m, "sigma_t", H5T_NATIVE_DOUBLE, space,
                       H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite(dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
                      H5P_DEFAULT, &mat->sigma_t(m)[0]);
    status = H5Dclose(dset);

    dset   = H5Dcreate(group_m, "sigma_f", H5T_NATIVE_DOUBLE, space,
                       H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite(dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
                      H5P_DEFAULT, &mat->sigma_f(m)[0]);
    status = H5Dclose(dset);

    dset   = H5Dcreate(group_m, "nu", H5T_NATIVE_DOUBLE, space,
                       H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite(dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
                      H5P_DEFAULT, &mat->nu(m)[0]);
    status = H5Dclose(dset);

    dset   = H5Dcreate(group_m, "chi", H5T_NATIVE_DOUBLE, space,
                       H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite(dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
                      H5P_DEFAULT, &mat->chi(m)[0]);
    status = H5Dclose(dset);

    dset   = H5Dcreate(group_m, "diff_coef", H5T_NATIVE_DOUBLE, space,
                       H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite(dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
                      H5P_DEFAULT, &mat->diff_coef(m)[0]);
    status = H5Dclose(dset);

    // SCATTER
    space  = H5Screate_simple(2, dims2, NULL);

    // There may be a better way to avoid so much copying.  Even so, this
    // is *not* intended to be an interface for large data.
    double ss[ng][ng];
    for (int g = 0; g < ng; g++)
      for (int gp = 0; gp < ng; gp++)
        ss[g][gp] = mat->sigma_s(m, g, gp);

    dset   = H5Dcreate(group_m, "sigma_s", H5T_NATIVE_DOUBLE, space,
                       H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite(dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
                      H5P_DEFAULT, ss);
    status = H5Dclose(dset);

    status = H5Sclose(space);
  }

  // Close the group.
  status = H5Gclose(group);

}

void IO_HDF5::close()
{
  // If not open, ignore.
  if (!d_open) return;

  // Close the HDF5 file
  H5Fclose(d_file_id);
  d_open = false;
}

void IO_HDF5::read(SP_input input)
{

  // Open the file if necessary.
  if (!d_open)
    d_file_id = H5Fopen(d_filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

  // Open the input group
  hid_t group = H5Gopen(d_file_id, "input", H5P_DEFAULT);

  // Read data
  read_data<int>(input, group, "int_data");
  read_data<double>(input, group, "dbl_data");
  read_data<std::string>(input, group, "str_data");
  read_data<vec_int>(input, group, "vec_int_data");
  read_data<vec_dbl>(input, group, "vec_dbl_data");

  // Close the group.
  herr_t status = H5Gclose(group);

}

} // end namespace detran_ioutils

#endif // DETRAN_ENABLE_HDF5

//---------------------------------------------------------------------------//
//              end of file IO_HDF5.cc
//---------------------------------------------------------------------------//
