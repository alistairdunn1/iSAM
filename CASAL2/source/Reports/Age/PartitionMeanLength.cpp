/**
 * @file PartitionMeanLength.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 7/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */

// headers
#include "PartitionMeanLength.h"

#include "AgeLengths/Manager.h"
#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Partition/Partition.h"
#include "Partition/Accessors/All.h"
#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace reports {
namespace age {

/**
 * Default constructor
 */
PartitionMeanLength::PartitionMeanLength(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection | RunMode::kSimulation | RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = (State::Type)(State::kIterationComplete);

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "The time step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for the report", "", true);
}

/**
 * Build method
 */
void PartitionMeanLength::DoBuild() {
  if (!parameters_.Get(PARAM_YEARS)->has_been_defined()) {
    years_ = model_->years();
  }
}

/**
 * Execute the report
 */
void PartitionMeanLength::DoExecute() {
  LOG_TRACE();

  unsigned year_index      = 0;
  unsigned time_step_index = model_->managers().time_step()->GetTimeStepIndex(time_step_);
  niwa::partition::accessors::All all_view(model_);

  cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
  cache_ << "time_step: " << time_step_ << "\n";

  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    string category = (*iterator)->name_;
    LOG_FINEST() << "printing mean length-at-age for category " << category;
    cache_ << category << " " << REPORT_R_LIST << "\n";

    for (auto year : years_) {
      year_index = year > model_->start_year() ? year - model_->start_year() : 0;
      cache_ << "year: " << year << "\n";

      cache_ << "mean_lengths " << REPORT_R_LIST << "\n";
      cache_ << "values: ";

      unsigned age_bins = (*iterator)->age_spread();
      for (unsigned age_index = 0; age_index < age_bins; ++age_index) {
        Double temp = (*iterator)->mean_length_by_time_step_age_[year_index][time_step_index][age_index];
        cache_ << AS_VALUE(temp) << " ";
      }
      cache_<<"\n";
      LOG_FINEST() << "cached mean length";
      cache_ << REPORT_R_LIST_END <<"\n";
    }

    cache_ << REPORT_R_LIST_END <<"\n";

    ready_for_writing_ = true;
  }

}

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */
