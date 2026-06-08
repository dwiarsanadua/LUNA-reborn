--[[
  Quest 393: [Special][Quest]A study of secret medicine
  Level Required: 60
  NPC Start: 92, NPC Complete: 92
  Prerequisites: [392]
]]

local fsm = require('fsm_engine')

local quest_393 = fsm:new({
    id = 393,
    name = "[Special][Quest]A study of secret medicine",
    level_required = 60,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {392},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 393: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 393: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 393: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 258,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 393: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 393: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 393: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 393: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 393: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 299, count = 1 },
                        { item_id = 299, count = 10 },
                        { item_id = 30000511, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_393