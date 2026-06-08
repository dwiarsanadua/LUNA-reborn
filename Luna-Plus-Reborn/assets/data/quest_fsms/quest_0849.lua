--[[
  Quest 849: The Rise of Evil
  Level Required: 87
  NPC Start: 125, NPC Complete: 505
]]

local fsm = require('fsm_engine')

local quest_849 = fsm:new({
    id = 849,
    name = "The Rise of Evil",
    level_required = 87,
    npc_start = 125,
    npc_complete = 505,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 849: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 849: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 849: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 849: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 849: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 502,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 849: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 849: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 849: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 849: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_849