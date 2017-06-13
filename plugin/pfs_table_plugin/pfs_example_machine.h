/* Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */

#ifndef PLUGIN_PFS_TABLE_PLUGIN_pfs_example_employee_MACHINE_H_
#define PLUGIN_PFS_TABLE_PLUGIN_pfs_example_employee_MACHINE_H_

#include <mysql/plugin.h>
#include <mysql/components/service_implementation.h>
#include <mysql/components/services/pfs_plugin_table_service.h>

/* Service handle */
extern SERVICE_TYPE(pfs_plugin_table) * table_svc;

/* Global share pointer for pfs_example_employee_salary table */
extern PFS_engine_table_share_proxy machine_st_share;

/* A mutex instance to protect:
 * - machine_rows_in_table
 * - next_available_index
 * - machine_records_array
 */
extern mysql_mutex_t LOCK_machine_records_array;

enum machine_type_enum
{
  LAPTOP = 1,
  DESKTOP = 2,
  MOBILE = 3,
  TYPE_END = 4
};

/* A structure to denote a single row of the table. */
struct
{
public:
  PSI_int machine_number;
  PSI_enum machine_type;
  char machine_made[20];
  unsigned int machine_made_length;
  PSI_int employee_number;

  /* If there is a value in this row */
  bool m_exist;
} typedef Machine_Record;

/**
 * An array to keep rows of the tables.
 * When a row is inserted in plugin table, it will be stored here.
 * When a row is queried from plugin table, it will be fetched from here.
 */
extern std::vector<Machine_Record> machine_records_vector;

/* A class to define position of cursor in table. */
class Machine_POS
{
private:
  unsigned int m_index;

public:
  ~Machine_POS(){};
  Machine_POS()
  {
    m_index = 0;
  }

  bool
  has_more()
  {
    if (m_index < machine_records_vector.size())
      return true;
    return false;
  }
  void
  next()
  {
    m_index++;
  }

  void
  reset()
  {
    m_index = 0;
  }

  unsigned int
  get_index()
  {
    return m_index;
  }

  void
  set_at(unsigned int index)
  {
    m_index = index;
  }

  void
  set_at(Machine_POS* pos)
  {
    m_index = pos->m_index;
  }

  void
  set_after(Machine_POS* pos)
  {
    m_index = pos->m_index + 1;
  }
};

/* A structure to define a handle for table in plugin/component code. */
typedef struct
{
  /* Current position instance */
  Machine_POS m_pos;
  /* Next position instance */
  Machine_POS m_next_pos;

  /* Current row for the table */
  Machine_Record current_row;
} Machine_Table_Handle;

PSI_table_handle* machine_open_table(PSI_pos** pos);
void machine_close_table(PSI_table_handle* handle);
int machine_rnd_next(PSI_table_handle* handle);
int machine_rnd_init(PSI_table_handle* h, bool scan);
int machine_rnd_pos(PSI_table_handle* handle);
int machine_index_init(PSI_table_handle* handle,
                       uint idx,
                       bool sorted,
                       PSI_index_handle** index);
int machine_index_read(PSI_index_handle* index,
                       PSI_key_reader* reader,
                       unsigned int idx,
                       int find_flag);
int machine_index_next(PSI_table_handle* handle);
void machine_reset_position(PSI_table_handle* handle);
int machine_read_column_value(PSI_table_handle* handle,
                              PSI_field* field,
                              uint index);
int machine_write_row_values(PSI_table_handle* handle);
int machine_write_column_value(PSI_table_handle* handle,
                               PSI_field* field,
                               unsigned int index);
int machine_update_row_values(PSI_table_handle* handle);
int machine_update_column_value(PSI_table_handle* handle,
                                PSI_field* field,
                                unsigned int index);
int machine_delete_row_values(PSI_table_handle* handle);
int machine_delete_all_rows(void);
unsigned long long machine_get_row_count(void);
void init_machine_share(PFS_engine_table_share_proxy* share);

#endif /* PLUGIN_PFS_TABLE_PLUGIN_pfs_example_employee_MACHINE_H_ */
