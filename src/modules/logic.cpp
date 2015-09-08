// Copyright 2015 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "modules/logic.h"
#include "event/event_system.h"
#include "event/base_node.h"

namespace fpl {
namespace fpl_project {

// clang-format off
#define LOGICAL_NODE(name, op)                                     \
  class name : public event::BaseNode {                            \
   public:                                                         \
    static void OnRegister(event::NodeSignature* node_sig) {       \
      node_sig->AddInput<bool>();                                  \
      node_sig->AddInput<bool>();                                  \
      node_sig->AddOutput<bool>();                                 \
    }                                                              \
                                                                   \
    virtual void Execute(event::Inputs* in, event::Outputs* out) { \
      auto a = in->Get<bool>(0);                                   \
      auto b = in->Get<bool>(1);                                   \
      out->Set(0, *a op *b);                                       \
    }                                                              \
  }
// clang-format on

LOGICAL_NODE(AndNode, &&);
LOGICAL_NODE(OrNode, ||);
LOGICAL_NODE(XorNode, ^);

// Logical Not.
class NotNode : public event::BaseNode {
 public:
  static void OnRegister(event::NodeSignature* node_sig) {
    node_sig->AddInput<bool>();
    node_sig->AddInput<bool>();
    node_sig->AddOutput<bool>();
  }

  virtual void Execute(event::Inputs* in, event::Outputs* out) {
    auto a = in->Get<bool>(0);
    out->Set(0, !*a);
  }
};

void InitializeLogicModule(event::EventSystem* event_system) {
  event::Module* module = event_system->AddModule("logic");
  module->RegisterNode<AndNode>("and");
  module->RegisterNode<OrNode>("or");
  module->RegisterNode<XorNode>("xor");
  module->RegisterNode<NotNode>("not");
}

}  // fpl_project
}  // fpl
