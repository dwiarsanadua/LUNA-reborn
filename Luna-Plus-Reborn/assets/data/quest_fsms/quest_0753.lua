--[[
  Quest 753: [Quest] The Joy of Cooking 2
  Level Required: 20
  NPC Start: 1, NPC Complete: 510
  Prerequisites: [754]
  Rewards: EXP=3733
]]

local fsm = require('fsm_engine')

local quest_753 = fsm:new({
    id = 753,
    name = "[Quest] The Joy of Cooking 2",
    level_required = 20,
    npc_start = 1,
    npc_complete = 510,
    prerequisites = {754},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 753: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 510,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 753: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 753: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 510,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 753: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 753: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 753: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 510,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 753: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 753: Rewards given")
                self:give_rewards({
                    exp = 3733,
                })
            end,
            transitions = {},
        },
    },
})

return quest_753