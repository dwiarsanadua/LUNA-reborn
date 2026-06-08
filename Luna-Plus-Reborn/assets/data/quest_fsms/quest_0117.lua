--[[
  Quest 117: Cursed Land
  Level Required: 23
  NPC Start: 26, NPC Complete: 26
  Rewards: EXP=10563, Gold=2715
]]

local fsm = require('fsm_engine')

local quest_117 = fsm:new({
    id = 117,
    name = "Cursed Land",
    level_required = 23,
    npc_start = 26,
    npc_complete = 26,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 117: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 117: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 117: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 45,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 117: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 53,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 117: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 117: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 117: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 117: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 117: Rewards given")
                self:give_rewards({
                    exp = 10563,
                    gold = 2715,
                    items = {
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_117