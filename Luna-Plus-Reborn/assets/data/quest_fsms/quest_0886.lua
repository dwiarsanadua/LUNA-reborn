--[[
  Quest 886: To Giant Garden
  Level Required: 96
  NPC Start: 129, NPC Complete: 540
]]

local fsm = require('fsm_engine')

local quest_886 = fsm:new({
    id = 886,
    name = "To Giant Garden",
    level_required = 96,
    npc_start = 129,
    npc_complete = 540,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 886: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 886: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 886: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 886: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 886: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 886: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 886: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 886: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 402, count = 1 },
                        { item_id = 402, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_886