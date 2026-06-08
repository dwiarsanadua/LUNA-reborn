--[[
  Quest 893: Dealing with Dryads
  Level Required: 97
  NPC Start: 540, NPC Complete: 540
  Rewards: EXP=8266754, Gold=77721
]]

local fsm = require('fsm_engine')

local quest_893 = fsm:new({
    id = 893,
    name = "Dealing with Dryads",
    level_required = 97,
    npc_start = 540,
    npc_complete = 540,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 893: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 893: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 893: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 330,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 893: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 331,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 893: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 893: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 893: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 893: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 893: Rewards given")
                self:give_rewards({
                    exp = 8266754,
                    gold = 77721,
                    items = {
                        { item_id = 21001505, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_893