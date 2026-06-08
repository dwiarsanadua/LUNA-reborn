--[[
  Quest 879: Can't Do enough
  Level Required: 94
  NPC Start: 91, NPC Complete: 91
  Prerequisites: [878]
  Rewards: EXP=5092601, Gold=61188
]]

local fsm = require('fsm_engine')

local quest_879 = fsm:new({
    id = 879,
    name = "Can't Do enough",
    level_required = 94,
    npc_start = 91,
    npc_complete = 91,
    prerequisites = {878},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 879: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 879: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 879: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 290,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 879: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 879: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 879: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 879: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 879: Rewards given")
                self:give_rewards({
                    exp = 5092601,
                    gold = 61188,
                })
            end,
            transitions = {},
        },
    },
})

return quest_879