/**
 * @file TimeStep.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

#include "TimeStep.h"

#include "InitialisationPhases/Manager.h"
#include "Model/Model.h"
#include "Observations/Manager.h"
#include "Processes/Manager.h"

namespace niwa {

/**
 * Default Constructor
 */
TimeStep::TimeStep() {
  LOG_TRACE();

  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_PROCESSES, &process_names_, "Processes", "");
}

/**
 * Validate our time step
 */
void TimeStep::Validate() {
  parameters_.Populate();
}

/**
 * Build our time step
 */
void TimeStep::Build() {

  // Get the pointers to our processes
  processes::Manager& process_manager = processes::Manager::Instance();
  for (string process_name : process_names_) {
    ProcessPtr process = process_manager.GetProcess(process_name);
    if (!process)
      LOG_ERROR_P(PARAM_PROCESSES) << ": process " << process_name << " does not exist. Have you defined it?";

    processes_.push_back(process);
  }
  LOG_FINE() << "Time step " << label_ << " has " << processes_.size() << " processes";

  /**
   * If we have a block build it
   */
  block_start_process_index_ = processes_.size();
  block_end_process_Index_ = processes_.size() - 1;
  for (unsigned i = 0; i < processes_.size(); ++i) {
    if (processes_[i]->process_type() == ProcessType::kMortality) {
      block_start_process_index_ = block_start_process_index_ == processes_.size() ? i : block_start_process_index_;
      block_end_process_Index_ = i;
    }
  }

  block_start_process_index_ = block_start_process_index_ == processes_.size() ? 0 : block_start_process_index_;
}

/**
 * Execute the time step during the initialisation phases
 */
void TimeStep::ExecuteForInitialisation(const string& phase_label) {
  LOG_FINE() << "Executing for initialisation phase: " << phase_label << " with " << initialisation_block_executors_.size() << " executors";
  LOG_FINE() << "initialisation_block_end_process_index_: " << initialisation_block_end_process_index_[phase_label];

  for (unsigned index = 0; index < initialisation_processes_[phase_label].size(); ++index) {
    initialisation_processes_[phase_label][index]->Execute(0u, "");
    if (initialisation_block_end_process_index_[phase_label] == index) {
      for (ExecutorPtr executor : initialisation_block_executors_) {
        executor->Execute();
      }
    }
  }
}

/**
 * Execute the time step
 */
void TimeStep::Execute(unsigned year) {
  LOG_TRACE();

  for (ExecutorPtr executor : executors_[year])
      executor->PreExecute();

  for (unsigned index = 0; index < processes_.size(); ++index) {
    if (index == block_start_process_index_) {
      for (ExecutorPtr executor : block_executors_[year])
        executor->PreExecute();
    }

    processes_[index]->Execute(year, label_);

    if (index == block_end_process_Index_) {
      for (ExecutorPtr executor : block_executors_[year])
        executor->Execute();
    }
  }

  for (ExecutorPtr executor : executors_[year])
    executor->Execute();
}

/**
 *
 */
void TimeStep::SubscribeToBlock(ExecutorPtr executor) {
  vector<unsigned> years = Model::Instance()->years();
  for (unsigned year : years)
    block_executors_[year].push_back(executor);
}

/**
 *
 */
void TimeStep::SetInitialisationProcessLabels(const string& initialisation_phase_label, vector<string> process_labels_) {
  initialisation_process_labels_[initialisation_phase_label] = process_labels_;
}

/**
 *
 */
void TimeStep::BuildInitialisationProcesses() {
  for (auto iter : initialisation_process_labels_) {
    for (string process_label : iter.second) {
      initialisation_processes_[iter.first].push_back(processes::Manager::Instance().GetProcess(process_label));
    }

    initialisation_block_end_process_index_[iter.first]   = iter.second.size() - 1;
    for (unsigned i = 0; i < initialisation_processes_[iter.first].size(); ++i) {
      if (initialisation_processes_[iter.first][i]->process_type() == ProcessType::kMortality) {
        initialisation_block_end_process_index_[iter.first] = i;
      }
    }
  }
}
} /* namespace niwa */
