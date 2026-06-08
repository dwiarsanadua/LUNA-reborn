--[[
  Quest 941: Shiny Wings
  Level Required: 105
  NPC Start: 542, NPC Complete: 542
  Prerequisites: [939]
  Rewards: EXP=15685431
]]

local fsm = require('fsm_engine')

local quest_941 = fsm:new({
    id = 941,
    name = "Shiny Wings",
    level_required = 105,
    npc_start = 542,
    npc_complete = 542,
    prerequisites = {939},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 941: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 941: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 941: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 325,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 941: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 941: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 941: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 941: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 941: Rewards given")
                self:give_rewards({
                    exp = 15685431,
                    items = {
                        { item_id = 409, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_941