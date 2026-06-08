--[[
  Quest 862: Birth of Life
  Level Required: 90
  NPC Start: 90, NPC Complete: 124
  Prerequisites: [861]
  Rewards: EXP=2473766
]]

local fsm = require('fsm_engine')

local quest_862 = fsm:new({
    id = 862,
    name = "Birth of Life",
    level_required = 90,
    npc_start = 90,
    npc_complete = 124,
    prerequisites = {861},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 862: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 862: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 862: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 862: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 862: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 862: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 862: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 862: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 862: Rewards given")
                self:give_rewards({
                    exp = 2473766,
                })
            end,
            transitions = {},
        },
    },
})

return quest_862