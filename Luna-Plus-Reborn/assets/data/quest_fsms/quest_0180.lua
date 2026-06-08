--[[
  Quest 180: First Strike
  Level Required: 38
  NPC Start: 37, NPC Complete: 37
  Prerequisites: [179]
  Rewards: EXP=76550, Gold=3880
]]

local fsm = require('fsm_engine')

local quest_180 = fsm:new({
    id = 180,
    name = "First Strike",
    level_required = 38,
    npc_start = 37,
    npc_complete = 37,
    prerequisites = {179},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 180: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 180: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 180: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 180: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 180: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 180: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 180: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 180: Rewards given")
                self:give_rewards({
                    exp = 76550,
                    gold = 3880,
                })
            end,
            transitions = {},
        },
    },
})

return quest_180