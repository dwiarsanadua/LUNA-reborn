--[[
  Quest 900: [Party] Heartwarming Theives 2
  Level Required: 99
  NPC Start: 538, NPC Complete: 538
  Prerequisites: [895]
  Rewards: EXP=9641761, Gold=77721
]]

local fsm = require('fsm_engine')

local quest_900 = fsm:new({
    id = 900,
    name = "[Party] Heartwarming Theives 2",
    level_required = 99,
    npc_start = 538,
    npc_complete = 538,
    prerequisites = {895},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 900: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 538,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 900: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 900: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 295,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 900: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 294,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 900: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 293,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 900: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 538,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 900: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 900: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 538,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 900: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 900: Rewards given")
                self:give_rewards({
                    exp = 9641761,
                    gold = 77721,
                })
            end,
            transitions = {},
        },
    },
})

return quest_900