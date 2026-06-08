--[[
  Quest 2044: [Daily] Sending Support 3
  Level Required: 94
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=1527780
]]

local fsm = require('fsm_engine')

local quest_2044 = fsm:new({
    id = 2044,
    name = "[Daily] Sending Support 3",
    level_required = 94,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2044: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2044: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2044: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2044: NPC talk objective met")
                    end
                },
                {
                    trigger = "any",
                    target = "COMPLETE",
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2044: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2044: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2044: Rewards given")
                self:give_rewards({
                    exp = 1527780,
                    items = {
                        { item_id = 30000836, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2044