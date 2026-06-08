--[[
  Quest 854: Want to see a friend
  Level Required: 88
  NPC Start: 128, NPC Complete: 129
]]

local fsm = require('fsm_engine')

local quest_854 = fsm:new({
    id = 854,
    name = "Want to see a friend",
    level_required = 88,
    npc_start = 128,
    npc_complete = 129,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 854: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 854: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 854: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 854: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 854: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 854: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 854: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 854: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_854