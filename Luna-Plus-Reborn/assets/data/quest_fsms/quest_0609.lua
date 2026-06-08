--[[
  Quest 609: Nera Castle
  Level Required: 80
  NPC Start: 6, NPC Complete: 127
  Rewards: EXP=806561
]]

local fsm = require('fsm_engine')

local quest_609 = fsm:new({
    id = 609,
    name = "Nera Castle",
    level_required = 80,
    npc_start = 6,
    npc_complete = 127,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 609: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 609: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 609: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 609: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 127,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 609: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 609: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 127,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 609: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 609: Rewards given")
                self:give_rewards({
                    exp = 806561,
                })
            end,
            transitions = {},
        },
    },
})

return quest_609