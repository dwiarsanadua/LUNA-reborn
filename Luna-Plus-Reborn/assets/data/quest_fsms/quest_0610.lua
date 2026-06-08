--[[
  Quest 610: Guild Challenge
  Level Required: 80
  NPC Start: 125, NPC Complete: 127
  Prerequisites: [609]
  Rewards: EXP=406561
]]

local fsm = require('fsm_engine')

local quest_610 = fsm:new({
    id = 610,
    name = "Guild Challenge",
    level_required = 80,
    npc_start = 125,
    npc_complete = 127,
    prerequisites = {609},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 610: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 127,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 610: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 610: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 127,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 610: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 610: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 610: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 127,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 610: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 610: Rewards given")
                self:give_rewards({
                    exp = 406561,
                })
            end,
            transitions = {},
        },
    },
})

return quest_610