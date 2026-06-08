--[[
  Quest 2023: [Daily] Troll Guard
  Level Required: 73
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=741488
]]

local fsm = require('fsm_engine')

local quest_2023 = fsm:new({
    id = 2023,
    name = "[Daily] Troll Guard",
    level_required = 73,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2023: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2023: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2023: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 105,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2023: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2023: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2023: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2023: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2023: Rewards given")
                self:give_rewards({
                    exp = 741488,
                    items = {
                        { item_id = 30000835, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2023