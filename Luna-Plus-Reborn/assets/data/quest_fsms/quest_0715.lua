--[[
  Quest 715: Note from Luna
  Level Required: 67
  NPC Start: 96, NPC Complete: 96
  Rewards: EXP=920321
]]

local fsm = require('fsm_engine')

local quest_715 = fsm:new({
    id = 715,
    name = "Note from Luna",
    level_required = 67,
    npc_start = 96,
    npc_complete = 96,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 715: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 96,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 715: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 715: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 641,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 715: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 96,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 715: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 715: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 96,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 715: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 715: Rewards given")
                self:give_rewards({
                    exp = 920321,
                })
            end,
            transitions = {},
        },
    },
})

return quest_715