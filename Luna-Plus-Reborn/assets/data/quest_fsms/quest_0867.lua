--[[
  Quest 867: Neat Finish
  Level Required: 91
  NPC Start: 92, NPC Complete: 92
  Prerequisites: [864]
  Rewards: EXP=4930862, Gold=61188
]]

local fsm = require('fsm_engine')

local quest_867 = fsm:new({
    id = 867,
    name = "Neat Finish",
    level_required = 91,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {864},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 867: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 867: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 867: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 288,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 867: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 867: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 867: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 867: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 867: Rewards given")
                self:give_rewards({
                    exp = 4930862,
                    gold = 61188,
                })
            end,
            transitions = {},
        },
    },
})

return quest_867