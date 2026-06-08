--[[
  Quest 611: The Secret Guild Challenge
  Level Required: 90
  NPC Start: 123, NPC Complete: 127
  Prerequisites: [609]
  Rewards: EXP=4065616
]]

local fsm = require('fsm_engine')

local quest_611 = fsm:new({
    id = 611,
    name = "The Secret Guild Challenge",
    level_required = 90,
    npc_start = 123,
    npc_complete = 127,
    prerequisites = {609},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 611: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 127,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 611: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 611: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 127,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 611: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 123,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 611: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 611: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 127,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 611: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 611: Rewards given")
                self:give_rewards({
                    exp = 4065616,
                })
            end,
            transitions = {},
        },
    },
})

return quest_611