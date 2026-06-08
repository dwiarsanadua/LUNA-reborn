--[[
  Quest 850: Wreck Madness 2
  Level Required: 87
  NPC Start: 502, NPC Complete: 502
  Prerequisites: [846]
  Rewards: EXP=3599379, Gold=57826
]]

local fsm = require('fsm_engine')

local quest_850 = fsm:new({
    id = 850,
    name = "Wreck Madness 2",
    level_required = 87,
    npc_start = 502,
    npc_complete = 502,
    prerequisites = {846},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 850: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 502,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 850: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 850: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 693,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 850: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 502,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 850: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 850: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 502,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 850: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 850: Rewards given")
                self:give_rewards({
                    exp = 3599379,
                    gold = 57826,
                })
            end,
            transitions = {},
        },
    },
})

return quest_850