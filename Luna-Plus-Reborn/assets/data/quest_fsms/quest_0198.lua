--[[
  Quest 198: Lost Keys
  Level Required: 44
  NPC Start: 62, NPC Complete: 109
  Prerequisites: [196]
  Rewards: EXP=165722, Gold=4500
]]

local fsm = require('fsm_engine')

local quest_198 = fsm:new({
    id = 198,
    name = "Lost Keys",
    level_required = 44,
    npc_start = 62,
    npc_complete = 109,
    prerequisites = {196},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 198: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 198: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 198: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 38,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 198: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 198: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 109,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 198: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 198: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 109,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 198: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 198: Rewards given")
                self:give_rewards({
                    exp = 165722,
                    gold = 4500,
                    items = {
                        { item_id = 135, count = 1 },
                        { item_id = 135, count = 10 },
                        { item_id = 136, count = 1 },
                        { item_id = 136, count = 1 },
                        { item_id = 21000009, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_198